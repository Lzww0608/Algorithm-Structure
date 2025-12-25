package safeudp

import (
	"runtime"
	"sync"
	"testing"
)

// MutexRingBuffer 使用互斥锁的传统实现（用于对比）
type MutexRingBuffer[T any] struct {
	mu     sync.Mutex
	buffer []T
	head   int
	tail   int
	size   int
}

func NewMutexRingBuffer[T any](capacity int) *MutexRingBuffer[T] {
	return &MutexRingBuffer[T]{
		buffer: make([]T, capacity),
		size:   capacity,
	}
}

func (rb *MutexRingBuffer[T]) Push(value T) bool {
	rb.mu.Lock()
	defer rb.mu.Unlock()

	next := (rb.tail + 1) % rb.size
	if next == rb.head {
		return false // 满了
	}
	rb.buffer[rb.tail] = value
	rb.tail = next
	return true
}

func (rb *MutexRingBuffer[T]) Pop() (T, bool) {
	rb.mu.Lock()
	defer rb.mu.Unlock()

	var zero T
	if rb.head == rb.tail {
		return zero, false // 空
	}
	value := rb.buffer[rb.head]
	rb.head = (rb.head + 1) % rb.size
	return value, true
}

// ChannelBuffer 使用channel的实现（用于对比）
type ChannelBuffer[T any] struct {
	ch chan T
}

func NewChannelBuffer[T any](capacity int) *ChannelBuffer[T] {
	return &ChannelBuffer[T]{
		ch: make(chan T, capacity),
	}
}

func (cb *ChannelBuffer[T]) Push(value T) bool {
	select {
	case cb.ch <- value:
		return true
	default:
		return false
	}
}

func (cb *ChannelBuffer[T]) Pop() (T, bool) {
	select {
	case value := <-cb.ch:
		return value, true
	default:
		var zero T
		return zero, false
	}
}

// BenchmarkComparison_LockFree_SPSC 单生产者单消费者：无锁版本
func BenchmarkComparison_LockFree_SPSC(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1024)
	b.ResetTimer()

	b.RunParallel(func(pb *testing.PB) {
		for pb.Next() {
			rb.Push(1)
			rb.Pop()
		}
	})
}

// BenchmarkComparison_Mutex_SPSC 单生产者单消费者：互斥锁版本
func BenchmarkComparison_Mutex_SPSC(b *testing.B) {
	rb := NewMutexRingBuffer[int](1024)
	b.ResetTimer()

	b.RunParallel(func(pb *testing.PB) {
		for pb.Next() {
			rb.Push(1)
			rb.Pop()
		}
	})
}

// BenchmarkComparison_Channel_SPSC 单生产者单消费者：channel版本
func BenchmarkComparison_Channel_SPSC(b *testing.B) {
	cb := NewChannelBuffer[int](1024)
	b.ResetTimer()

	b.RunParallel(func(pb *testing.PB) {
		for pb.Next() {
			cb.Push(1)
			cb.Pop()
		}
	})
}

// BenchmarkComparison_LockFree_MPMC 多生产者多消费者：无锁版本
func BenchmarkComparison_LockFree_MPMC(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](4096)
	numProducers := runtime.NumCPU() / 2
	numConsumers := runtime.NumCPU() / 2
	if numProducers == 0 {
		numProducers = 1
	}
	if numConsumers == 0 {
		numConsumers = 1
	}

	b.ResetTimer()
	var wg sync.WaitGroup

	// 生产者
	for p := 0; p < numProducers; p++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < b.N/numProducers; i++ {
				for !rb.Push(i) {
					runtime.Gosched()
				}
			}
		}()
	}

	// 消费者
	for c := 0; c < numConsumers; c++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			count := 0
			target := b.N / numConsumers
			for count < target {
				if _, ok := rb.Pop(); ok {
					count++
				} else {
					runtime.Gosched()
				}
			}
		}()
	}

	wg.Wait()
}

// BenchmarkComparison_Mutex_MPMC 多生产者多消费者：互斥锁版本
func BenchmarkComparison_Mutex_MPMC(b *testing.B) {
	rb := NewMutexRingBuffer[int](4096)
	numProducers := runtime.NumCPU() / 2
	numConsumers := runtime.NumCPU() / 2
	if numProducers == 0 {
		numProducers = 1
	}
	if numConsumers == 0 {
		numConsumers = 1
	}

	b.ResetTimer()
	var wg sync.WaitGroup

	// 生产者
	for p := 0; p < numProducers; p++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < b.N/numProducers; i++ {
				for !rb.Push(i) {
					runtime.Gosched()
				}
			}
		}()
	}

	// 消费者
	for c := 0; c < numConsumers; c++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			count := 0
			target := b.N / numConsumers
			for count < target {
				if _, ok := rb.Pop(); ok {
					count++
				} else {
					runtime.Gosched()
				}
			}
		}()
	}

	wg.Wait()
}

// BenchmarkComparison_Channel_MPMC 多生产者多消费者：channel版本
func BenchmarkComparison_Channel_MPMC(b *testing.B) {
	cb := NewChannelBuffer[int](4096)
	numProducers := runtime.NumCPU() / 2
	numConsumers := runtime.NumCPU() / 2
	if numProducers == 0 {
		numProducers = 1
	}
	if numConsumers == 0 {
		numConsumers = 1
	}

	b.ResetTimer()
	var wg sync.WaitGroup

	// 生产者
	for p := 0; p < numProducers; p++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < b.N/numProducers; i++ {
				for !cb.Push(i) {
					runtime.Gosched()
				}
			}
		}()
	}

	// 消费者
	for c := 0; c < numConsumers; c++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			count := 0
			target := b.N / numConsumers
			for count < target {
				if _, ok := cb.Pop(); ok {
					count++
				} else {
					runtime.Gosched()
				}
			}
		}()
	}

	wg.Wait()
}

// BenchmarkComparison_LockFree_HighContention 高竞争场景：无锁版本
func BenchmarkComparison_LockFree_HighContention(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](64) // 小容量增加竞争
	numWorkers := runtime.NumCPU()

	b.ResetTimer()
	var wg sync.WaitGroup

	for w := 0; w < numWorkers; w++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			for i := 0; i < b.N/numWorkers; i++ {
				if id%2 == 0 {
					for !rb.Push(i) {
						runtime.Gosched()
					}
				} else {
					for {
						if _, ok := rb.Pop(); ok {
							break
						}
						runtime.Gosched()
					}
				}
			}
		}(w)
	}

	wg.Wait()
}

// BenchmarkComparison_Mutex_HighContention 高竞争场景：互斥锁版本
func BenchmarkComparison_Mutex_HighContention(b *testing.B) {
	rb := NewMutexRingBuffer[int](64) // 小容量增加竞争
	numWorkers := runtime.NumCPU()

	b.ResetTimer()
	var wg sync.WaitGroup

	for w := 0; w < numWorkers; w++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			for i := 0; i < b.N/numWorkers; i++ {
				if id%2 == 0 {
					for !rb.Push(i) {
						runtime.Gosched()
					}
				} else {
					for {
						if _, ok := rb.Pop(); ok {
							break
						}
						runtime.Gosched()
					}
				}
			}
		}(w)
	}

	wg.Wait()
}

// BenchmarkComparison_LockFree_PushOnly 只写：无锁版本
func BenchmarkComparison_LockFree_PushOnly(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1048576) // 大容量避免满
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		rb.Push(i)
	}
}

// BenchmarkComparison_Mutex_PushOnly 只写：互斥锁版本
func BenchmarkComparison_Mutex_PushOnly(b *testing.B) {
	rb := NewMutexRingBuffer[int](1048576) // 大容量避免满
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		rb.Push(i)
	}
}

// BenchmarkComparison_LockFree_PopOnly 只读：无锁版本
func BenchmarkComparison_LockFree_PopOnly(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1048576)
	// 预填充
	for i := 0; i < 1048576; i++ {
		rb.Push(i)
	}
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if _, ok := rb.Pop(); !ok {
			// 重新填充
			for j := 0; j < 1024; j++ {
				rb.Push(j)
			}
		}
	}
}

// BenchmarkComparison_Mutex_PopOnly 只读：互斥锁版本
func BenchmarkComparison_Mutex_PopOnly(b *testing.B) {
	rb := NewMutexRingBuffer[int](1048576)
	// 预填充
	for i := 0; i < 1048576; i++ {
		rb.Push(i)
	}
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if _, ok := rb.Pop(); !ok {
			// 重新填充
			for j := 0; j < 1024; j++ {
				rb.Push(j)
			}
		}
	}
}
