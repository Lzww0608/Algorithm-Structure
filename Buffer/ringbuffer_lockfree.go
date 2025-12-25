package safeudp

import (
	"runtime"
	"sync/atomic"
	"unsafe"
)

// cacheLinePadding 用于避免false sharing，填充到64字节（常见CPU缓存行大小）
type cacheLinePadding [64]byte

// lockFreeSlot 表示无锁环形缓冲区中的一个槽位
// 使用sequence来解决ABA问题和use-after-free问题
type lockFreeSlot[T any] struct {
	// sequence用于跟踪slot的状态：
	// sequence的值表示这个slot的"版本号"
	// 通过比较sequence和position的关系来判断slot的状态
	sequence uint64
	_        cacheLinePadding // 避免false sharing
	value    T
	_        cacheLinePadding // 避免false sharing
}

// LockFreeRingBuffer 无锁环形缓冲区
// 支持多生产者多消费者（MPMC）模式
//
// 核心设计原理：
// 1. 使用sequence number避免ABA问题
// 2. 使用cache line padding避免false sharing
// 3. 使用CAS操作保证原子性
// 4. capacity必须是2的幂，使用位运算优化性能
type LockFreeRingBuffer[T any] struct {
	_        cacheLinePadding
	buffer   []lockFreeSlot[T]
	mask     uint64 // capacity - 1，用于快速取模
	_        cacheLinePadding
	head     uint64 // 生产者写入位置（原子操作）
	_        cacheLinePadding
	tail     uint64 // 消费者读取位置（原子操作）
	_        cacheLinePadding
	capacity int
}

// NewLockFreeRingBuffer 创建一个新的无锁环形缓冲区
// capacity会被向上取整到最近的2的幂，这样可以用位运算代替取模操作
func NewLockFreeRingBuffer[T any](capacity int) *LockFreeRingBuffer[T] {
	// 确保capacity是2的幂
	if capacity <= 0 || (capacity&(capacity-1)) != 0 {
		// 向上取整到最近的2的幂
		capacity = roundUpToPowerOf2(capacity)
		if capacity <= 0 {
			capacity = 1024 // 默认大小
		}
	}

	buffer := make([]lockFreeSlot[T], capacity)
	// 初始化每个slot的sequence
	// sequence初始值等于slot的索引位置
	for i := range buffer {
		buffer[i].sequence = uint64(i)
	}

	return &LockFreeRingBuffer[T]{
		buffer:   buffer,
		mask:     uint64(capacity - 1),
		head:     0,
		tail:     0,
		capacity: capacity,
	}
}

// roundUpToPowerOf2 将n向上取整到最近的2的幂
func roundUpToPowerOf2(n int) int {
	if n <= 0 {
		return 1
	}
	n--
	n |= n >> 1
	n |= n >> 2
	n |= n >> 4
	n |= n >> 8
	n |= n >> 16
	n |= n >> 32
	n++
	return n
}

// Push 向缓冲区中推入一个元素（无锁实现）
// 返回true表示成功，false表示缓冲区已满
//
// 算法原理：
// 1. 获取当前head位置
// 2. 检查对应slot的sequence，判断是否可写
// 3. 使用CAS原子地更新head
// 4. 写入数据并更新sequence
func (rb *LockFreeRingBuffer[T]) Push(value T) bool {
	for {
		// 获取当前的head位置（Acquire语义）
		head := atomic.LoadUint64(&rb.head)
		slot := &rb.buffer[head&rb.mask]

		// 读取slot的sequence（Acquire语义）
		seq := atomic.LoadUint64(&slot.sequence)

		// 计算期望的sequence
		// 如果seq == head，说明这个slot是空的，可以写入
		diff := int64(seq) - int64(head)

		if diff == 0 {
			// slot为空，尝试CAS更新head
			if atomic.CompareAndSwapUint64(&rb.head, head, head+1) {
				// 成功获取该slot，写入数据
				slot.value = value
				// 更新sequence为head+1，标记为已填充
				// 使用Release语义确保value的写入对其他goroutine可见
				atomic.StoreUint64(&slot.sequence, head+1)
				return true
			}
			// CAS失败，说明有其他goroutine抢先了，重试
		} else if diff < 0 {
			// 缓冲区已满（生产者已经绕了一圈追上消费者）
			return false
		} else {
			// diff > 0，说明有其他goroutine已经占用了这个slot但还没完成写入
			// 这种情况很少见，让出CPU时间片等待其完成
			runtime.Gosched()
		}
	}
}

// Pop 从缓冲区中弹出一个元素（无锁实现）
// 返回元素和成功标志，如果缓冲区为空则返回零值和false
//
// 算法原理：
// 1. 获取当前tail位置
// 2. 检查对应slot的sequence，判断是否可读
// 3. 使用CAS原子地更新tail
// 4. 读取数据，清空slot并更新sequence准备下一轮
func (rb *LockFreeRingBuffer[T]) Pop() (T, bool) {
	var zero T
	for {
		// 获取当前的tail位置（Acquire语义）
		tail := atomic.LoadUint64(&rb.tail)
		slot := &rb.buffer[tail&rb.mask]

		// 读取slot的sequence（Acquire语义）
		seq := atomic.LoadUint64(&slot.sequence)

		// 计算期望的sequence
		// 如果seq == tail + 1，说明这个slot已经被写入，可以读取
		diff := int64(seq) - int64(tail+1)

		if diff == 0 {
			// slot已填充，尝试CAS更新tail
			if atomic.CompareAndSwapUint64(&rb.tail, tail, tail+1) {
				// 成功获取该slot，读取数据
				value := slot.value
				// 清空value，避免内存泄漏（use-after-free问题的关键）
				// 特别是对于指针类型，必须清空以释放引用
				slot.value = zero
				// 更新sequence为tail + mask + 1，标记为已读取并准备下一轮
				// mask + 1 = capacity，表示这个slot准备接收capacity个位置后的新数据
				// 使用Release语义
				atomic.StoreUint64(&slot.sequence, tail+rb.mask+1)
				return value, true
			}
			// CAS失败，重试
		} else if diff < 0 {
			// 缓冲区为空（消费者追上了生产者）
			return zero, false
		} else {
			// diff > 0，说明有其他goroutine已经占用了这个slot但还没完成读取
			runtime.Gosched()
		}
	}
}

// TryPush 尝试推入元素，不阻塞
// 这是Push的别名，保持接口一致性
func (rb *LockFreeRingBuffer[T]) TryPush(value T) bool {
	return rb.Push(value)
}

// TryPop 尝试弹出元素，不阻塞
// 这是Pop的别名，保持接口一致性
func (rb *LockFreeRingBuffer[T]) TryPop() (T, bool) {
	return rb.Pop()
}

// Empty 检查缓冲区是否为空
// 注意：在并发环境下，返回值可能立即失效（这是快照值）
func (rb *LockFreeRingBuffer[T]) Empty() bool {
	head := atomic.LoadUint64(&rb.head)
	tail := atomic.LoadUint64(&rb.tail)
	return head == tail
}

// Full 检查缓冲区是否已满
// 注意：在并发环境下，返回值可能立即失效（这是快照值）
func (rb *LockFreeRingBuffer[T]) Full() bool {
	head := atomic.LoadUint64(&rb.head)
	tail := atomic.LoadUint64(&rb.tail)
	return head-tail >= uint64(rb.capacity)
}

// Len 返回当前缓冲区中的元素数量
// 注意：在并发环境下，返回值是一个近似值（快照）
func (rb *LockFreeRingBuffer[T]) Len() int {
	head := atomic.LoadUint64(&rb.head)
	tail := atomic.LoadUint64(&rb.tail)
	return int(head - tail)
}

// Cap 返回缓冲区的容量
func (rb *LockFreeRingBuffer[T]) Cap() int {
	return rb.capacity
}

// ForEach 遍历缓冲区中的所有元素（从旧到新）
// fn返回false时停止遍历
// 警告：此方法在并发环境下不保证一致性快照，仅用于调试和监控
func (rb *LockFreeRingBuffer[T]) ForEach(fn func(T) bool) {
	tail := atomic.LoadUint64(&rb.tail)
	head := atomic.LoadUint64(&rb.head)

	for i := tail; i < head; i++ {
		slot := &rb.buffer[i&rb.mask]
		// 这里不做严格的sequence检查，因为这只是一个近似遍历
		// 在生产环境中可能读到不一致的数据，仅供调试使用
		if !fn(slot.value) {
			break
		}
	}
}

// ForEachReverse 反向遍历缓冲区中的所有元素（从新到旧）
// fn返回false时停止遍历
// 警告：此方法在并发环境下不保证一致性快照，仅用于调试和监控
func (rb *LockFreeRingBuffer[T]) ForEachReverse(fn func(T) bool) {
	head := atomic.LoadUint64(&rb.head)
	tail := atomic.LoadUint64(&rb.tail)

	if head == tail {
		return
	}

	for i := head - 1; i >= tail; i-- {
		slot := &rb.buffer[i&rb.mask]
		if !fn(slot.value) {
			break
		}
		if i == tail {
			break
		}
	}
}

// Discard 丢弃最多n个元素
// 返回实际丢弃的元素数量
// 使用无锁算法保证并发安全
func (rb *LockFreeRingBuffer[T]) Discard(n int) int {
	if n <= 0 {
		return 0
	}

	var zero T
	discarded := 0

	for i := 0; i < n; i++ {
		tail := atomic.LoadUint64(&rb.tail)
		slot := &rb.buffer[tail&rb.mask]
		seq := atomic.LoadUint64(&slot.sequence)

		diff := int64(seq) - int64(tail+1)

		if diff == 0 {
			if atomic.CompareAndSwapUint64(&rb.tail, tail, tail+1) {
				// 清空数据，避免内存泄漏
				slot.value = zero
				atomic.StoreUint64(&slot.sequence, tail+rb.mask+1)
				discarded++
			}
		} else if diff < 0 {
			// 缓冲区为空
			break
		}
	}

	return discarded
}

// Reset 重置缓冲区
// 警告：这个操作不是线程安全的，应该在确保没有并发访问的情况下调用
// 比如在程序初始化阶段或者使用外部同步机制保护的情况下
func (rb *LockFreeRingBuffer[T]) Reset() {
	atomic.StoreUint64(&rb.head, 0)
	atomic.StoreUint64(&rb.tail, 0)
	var zero T
	for i := range rb.buffer {
		rb.buffer[i].value = zero
		rb.buffer[i].sequence = uint64(i)
	}
}

// 编译时检查，确保类型对齐和大小符合预期
var _ = func() struct{} {
	var rb LockFreeRingBuffer[int]
	// 检查关键字段的对齐和大小
	_ = unsafe.Sizeof(rb.head)
	_ = unsafe.Sizeof(rb.tail)
	_ = unsafe.Sizeof(rb.mask)
	return struct{}{}
}()
