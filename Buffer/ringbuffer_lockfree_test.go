package safeudp

import (
	"runtime"
	"sync"
	"sync/atomic"
	"testing"
	"time"
)

// TestLockFreeRingBufferBasic 基本功能测试
func TestLockFreeRingBufferBasic(t *testing.T) {
	rb := NewLockFreeRingBuffer[int](8)

	// 测试初始状态
	if !rb.Empty() {
		t.Error("新建的buffer应该是空的")
	}
	if rb.Len() != 0 {
		t.Errorf("期望长度0，得到%d", rb.Len())
	}
	if rb.Cap() != 8 {
		t.Errorf("期望容量8，得到%d", rb.Cap())
	}

	// 测试Push
	for i := 0; i < 5; i++ {
		if !rb.Push(i) {
			t.Errorf("Push %d 失败", i)
		}
	}
	if rb.Len() != 5 {
		t.Errorf("期望长度5，得到%d", rb.Len())
	}

	// 测试Pop
	for i := 0; i < 5; i++ {
		val, ok := rb.Pop()
		if !ok {
			t.Errorf("Pop失败在索引%d", i)
		}
		if val != i {
			t.Errorf("期望值%d，得到%d", i, val)
		}
	}

	// 测试空buffer的Pop
	_, ok := rb.Pop()
	if ok {
		t.Error("从空buffer Pop应该失败")
	}
}

// TestLockFreeRingBufferFull 测试满buffer的情况
func TestLockFreeRingBufferFull(t *testing.T) {
	capacity := 8
	rb := NewLockFreeRingBuffer[int](capacity)

	// 填满buffer
	for i := 0; i < capacity; i++ {
		if !rb.Push(i) {
			t.Errorf("Push %d 失败", i)
		}
	}

	// 尝试push到满buffer
	if rb.Push(999) {
		t.Error("向满buffer Push应该失败")
	}

	// Pop一个元素
	val, ok := rb.Pop()
	if !ok || val != 0 {
		t.Errorf("Pop失败，期望0，得到%d", val)
	}

	// 现在应该可以再push一个
	if !rb.Push(999) {
		t.Error("Pop后应该可以Push")
	}
}

// TestLockFreeRingBufferConcurrent 并发测试
func TestLockFreeRingBufferConcurrent(t *testing.T) {
	rb := NewLockFreeRingBuffer[int](1024)
	numProducers := 4
	numConsumers := 4
	numItems := 10000

	var wg sync.WaitGroup
	var pushCount, popCount int64
	done := make(chan struct{})
	var closeOnce sync.Once

	// 启动生产者
	for p := 0; p < numProducers; p++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			for i := 0; i < numItems; i++ {
				value := id*numItems + i
				for !rb.Push(value) {
					runtime.Gosched()
				}
				atomic.AddInt64(&pushCount, 1)
			}
		}(p)
	}

	// 启动消费者
	for c := 0; c < numConsumers; c++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for {
				select {
				case <-done:
					return
				default:
					if val, ok := rb.Pop(); ok {
						_ = val
						if atomic.AddInt64(&popCount, 1) >= int64(numProducers*numItems) {
							closeOnce.Do(func() { close(done) })
							return
						}
					} else {
						runtime.Gosched()
					}
				}
			}
		}()
	}

	wg.Wait()

	// 验证所有数据都被处理
	if pushCount != int64(numProducers*numItems) {
		t.Errorf("期望push %d 次，实际 %d 次", numProducers*numItems, pushCount)
	}
	if popCount != int64(numProducers*numItems) {
		t.Errorf("期望pop %d 次，实际 %d 次", numProducers*numItems, popCount)
	}
	if !rb.Empty() {
		t.Errorf("测试结束后buffer应该为空，但还有 %d 个元素", rb.Len())
	}
}

// TestLockFreeRingBufferNoDataLoss 测试数据不丢失
func TestLockFreeRingBufferNoDataLoss(t *testing.T) {
	rb := NewLockFreeRingBuffer[int](128)
	numProducers := 8
	numItems := 1000

	var wg sync.WaitGroup
	pushed := make(map[int]int)
	popped := make(map[int]int)
	var mu sync.Mutex

	// 生产者
	for p := 0; p < numProducers; p++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			for i := 0; i < numItems; i++ {
				value := id*numItems + i
				for !rb.Push(value) {
					runtime.Gosched()
				}
				mu.Lock()
				pushed[value]++
				mu.Unlock()
			}
		}(p)
	}

	// 消费者
	totalItems := numProducers * numItems
	wg.Add(1)
	go func() {
		defer wg.Done()
		count := 0
		for count < totalItems {
			if val, ok := rb.Pop(); ok {
				mu.Lock()
				popped[val]++
				mu.Unlock()
				count++
			} else {
				runtime.Gosched()
			}
		}
	}()

	wg.Wait()

	// 验证每个值都被push和pop了一次
	for value, pushCnt := range pushed {
		if pushCnt != 1 {
			t.Errorf("值 %d 被push了 %d 次", value, pushCnt)
		}
		if popCnt, ok := popped[value]; !ok || popCnt != 1 {
			t.Errorf("值 %d 被pop了 %d 次", value, popCnt)
		}
	}
}

// TestLockFreeRingBufferDiscard 测试Discard功能
func TestLockFreeRingBufferDiscard(t *testing.T) {
	rb := NewLockFreeRingBuffer[int](8)

	// Push一些数据
	for i := 0; i < 5; i++ {
		rb.Push(i)
	}

	// Discard 2个元素
	discarded := rb.Discard(2)
	if discarded != 2 {
		t.Errorf("期望丢弃2个，实际丢弃%d个", discarded)
	}
	if rb.Len() != 3 {
		t.Errorf("期望长度3，得到%d", rb.Len())
	}

	// 验证剩余的元素
	val, _ := rb.Pop()
	if val != 2 {
		t.Errorf("期望值2，得到%d", val)
	}
}

// TestLockFreeRingBufferForEach 测试ForEach功能
func TestLockFreeRingBufferForEach(t *testing.T) {
	rb := NewLockFreeRingBuffer[int](8)

	// Push一些数据
	for i := 0; i < 5; i++ {
		rb.Push(i)
	}

	// 测试ForEach
	collected := []int{}
	rb.ForEach(func(val int) bool {
		collected = append(collected, val)
		return true
	})

	if len(collected) != 5 {
		t.Errorf("期望收集5个元素，得到%d个", len(collected))
	}
	for i, val := range collected {
		if val != i {
			t.Errorf("索引%d：期望值%d，得到%d", i, i, val)
		}
	}
}

// TestLockFreeRingBufferStress 压力测试
func TestLockFreeRingBufferStress(t *testing.T) {
	if testing.Short() {
		t.Skip("跳过压力测试")
	}

	rb := NewLockFreeRingBuffer[int](512)
	duration := 2 * time.Second
	numGoroutines := runtime.NumCPU() * 2

	var wg sync.WaitGroup
	stop := make(chan struct{})
	var operations int64

	// 启动读写goroutine
	for i := 0; i < numGoroutines; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			for {
				select {
				case <-stop:
					return
				default:
					if id%2 == 0 {
						rb.Push(id)
					} else {
						rb.Pop()
					}
					atomic.AddInt64(&operations, 1)
				}
			}
		}(i)
	}

	// 运行指定时间
	time.Sleep(duration)
	close(stop)
	wg.Wait()

	t.Logf("压力测试完成: %d 次操作在 %v", operations, duration)
	t.Logf("操作速率: %.2f ops/sec", float64(operations)/duration.Seconds())
}

// TestLockFreeRingBufferMemoryLeak 测试内存泄漏
func TestLockFreeRingBufferMemoryLeak(t *testing.T) {
	type LeakTest struct {
		data [1024]byte
	}

	rb := NewLockFreeRingBuffer[*LeakTest](16)

	// Push和Pop很多次
	for i := 0; i < 10000; i++ {
		obj := &LeakTest{}
		rb.Push(obj)
		rb.Pop()
	}

	// 验证buffer是空的
	if !rb.Empty() {
		t.Error("buffer应该是空的")
	}

	// 注意：真正的内存泄漏检测需要使用pprof等工具
	// 这里只是基本的验证
}

// BenchmarkLockFreeRingBufferPush Push性能基准测试
func BenchmarkLockFreeRingBufferPush(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1024)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		if !rb.Push(i) {
			rb.Pop() // 满了就pop一个
			rb.Push(i)
		}
	}
}

// BenchmarkLockFreeRingBufferPop Pop性能基准测试
func BenchmarkLockFreeRingBufferPop(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1024)
	// 预先填充
	for i := 0; i < 1024; i++ {
		rb.Push(i)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		if _, ok := rb.Pop(); !ok {
			// 空了就重新填充
			for j := 0; j < 1024; j++ {
				rb.Push(j)
			}
			rb.Pop()
		}
	}
}

// BenchmarkLockFreeRingBufferConcurrent 并发性能基准测试
func BenchmarkLockFreeRingBufferConcurrent(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1024)

	b.ResetTimer()
	b.RunParallel(func(pb *testing.PB) {
		for pb.Next() {
			if rb.Push(1) {
				rb.Pop()
			}
		}
	})
}

// BenchmarkLockFreeRingBufferMPMC 多生产者多消费者基准测试
func BenchmarkLockFreeRingBufferMPMC(b *testing.B) {
	rb := NewLockFreeRingBuffer[int](1024)
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
