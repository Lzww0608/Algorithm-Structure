package safeudp

import (
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
)

// ExampleBasicUsage 基本使用示例
func ExampleBasicUsage() {
	// 创建容量为1024的无锁ringbuffer（会自动向上取整到2的幂）
	rb := NewLockFreeRingBuffer[int](1000) // 实际容量1024

	// Push操作
	for i := 0; i < 10; i++ {
		if rb.Push(i) {
			fmt.Printf("成功推入: %d\n", i)
		} else {
			fmt.Println("Buffer已满")
		}
	}

	// Pop操作
	for i := 0; i < 10; i++ {
		if val, ok := rb.Pop(); ok {
			fmt.Printf("成功弹出: %d\n", val)
		} else {
			fmt.Println("Buffer为空")
		}
	}

	// 查询状态
	fmt.Printf("当前长度: %d\n", rb.Len())
	fmt.Printf("容量: %d\n", rb.Cap())
	fmt.Printf("是否为空: %v\n", rb.Empty())
}

// ExampleProducerConsumer 单生产者单消费者模式
func ExampleProducerConsumer() {
	rb := NewLockFreeRingBuffer[string](512)
	var wg sync.WaitGroup

	// 生产者
	wg.Add(1)
	go func() {
		defer wg.Done()
		for i := 0; i < 1000; i++ {
			msg := fmt.Sprintf("消息-%d", i)
			for !rb.Push(msg) {
				runtime.Gosched() // Buffer满时让出CPU
			}
		}
		fmt.Println("生产者完成")
	}()

	// 消费者
	wg.Add(1)
	go func() {
		defer wg.Done()
		count := 0
		for count < 1000 {
			if msg, ok := rb.Pop(); ok {
				// 处理消息
				_ = msg
				count++
			} else {
				runtime.Gosched() // Buffer空时让出CPU
			}
		}
		fmt.Printf("消费者完成，处理了%d条消息\n", count)
	}()

	wg.Wait()
}

// ExampleMultiProducerMultiConsumer 多生产者多消费者模式（MPMC）
func ExampleMultiProducerMultiConsumer() {
	rb := NewLockFreeRingBuffer[int](1024)
	numProducers := 4
	numConsumers := 4
	itemsPerProducer := 10000

	var wg sync.WaitGroup
	var totalProduced, totalConsumed int64

	// 启动多个生产者
	for p := 0; p < numProducers; p++ {
		wg.Add(1)
		go func(producerID int) {
			defer wg.Done()
			for i := 0; i < itemsPerProducer; i++ {
				data := producerID*itemsPerProducer + i
				for !rb.Push(data) {
					runtime.Gosched()
				}
				atomic.AddInt64(&totalProduced, 1)
			}
			fmt.Printf("生产者%d完成\n", producerID)
		}(p)
	}

	// 启动多个消费者
	targetTotal := int64(numProducers * itemsPerProducer)
	done := make(chan struct{})
	var closeOnce sync.Once

	for c := 0; c < numConsumers; c++ {
		wg.Add(1)
		go func(consumerID int) {
			defer wg.Done()
			localCount := 0
			for {
				select {
				case <-done:
					fmt.Printf("消费者%d完成，处理了%d条\n", consumerID, localCount)
					return
				default:
					if data, ok := rb.Pop(); ok {
						// 处理数据
						_ = data
						localCount++
						if atomic.AddInt64(&totalConsumed, 1) >= targetTotal {
							closeOnce.Do(func() { close(done) })
							return
						}
					} else {
						runtime.Gosched()
					}
				}
			}
		}(c)
	}

	wg.Wait()
	fmt.Printf("总共生产: %d, 总共消费: %d\n", totalProduced, totalConsumed)
}

// ExampleWithTimeout 带超时的读写示例
func ExampleWithTimeout() {
	rb := NewLockFreeRingBuffer[int](16)

	// 带超时的Push
	pushWithTimeout := func(value int, timeout time.Duration) bool {
		deadline := time.Now().Add(timeout)
		for {
			if rb.Push(value) {
				return true
			}
			if time.Now().After(deadline) {
				return false
			}
			time.Sleep(time.Microsecond)
		}
	}

	// 带超时的Pop
	popWithTimeout := func(timeout time.Duration) (int, bool) {
		deadline := time.Now().Add(timeout)
		for {
			if val, ok := rb.Pop(); ok {
				return val, true
			}
			if time.Now().After(deadline) {
				return 0, false
			}
			time.Sleep(time.Microsecond)
		}
	}

	// 使用示例
	if pushWithTimeout(42, 100*time.Millisecond) {
		fmt.Println("Push成功")
	} else {
		fmt.Println("Push超时")
	}

	if val, ok := popWithTimeout(100 * time.Millisecond); ok {
		fmt.Printf("Pop成功: %d\n", val)
	} else {
		fmt.Println("Pop超时")
	}
}

// ExampleBatchProcessing 批量处理示例
func ExampleBatchProcessing() {
	rb := NewLockFreeRingBuffer[int](4096)
	batchSize := 100

	var wg sync.WaitGroup

	// 批量生产者
	wg.Add(1)
	go func() {
		defer wg.Done()
		batch := make([]int, batchSize)
		for round := 0; round < 10; round++ {
			// 准备批次数据
			for i := 0; i < batchSize; i++ {
				batch[i] = round*batchSize + i
			}
			// 批量推入
			for _, item := range batch {
				for !rb.Push(item) {
					runtime.Gosched()
				}
			}
			fmt.Printf("推入批次%d，%d个元素\n", round, batchSize)
		}
	}()

	// 批量消费者
	wg.Add(1)
	go func() {
		defer wg.Done()
		batch := make([]int, 0, batchSize)
		total := 10 * batchSize
		consumed := 0

		for consumed < total {
			// 收集一批数据
			for len(batch) < batchSize && consumed < total {
				if val, ok := rb.Pop(); ok {
					batch = append(batch, val)
					consumed++
				} else {
					if len(batch) > 0 {
						break // 有数据就处理
					}
					runtime.Gosched()
				}
			}

			// 批量处理
			if len(batch) > 0 {
				// 处理整个批次
				processedSum := 0
				for _, val := range batch {
					processedSum += val
				}
				fmt.Printf("处理批次: %d个元素，总和=%d\n", len(batch), processedSum)
				batch = batch[:0] // 重置批次
			}
		}
	}()

	wg.Wait()
}

// ExampleMonitoring 监控示例
func ExampleMonitoring() {
	rb := NewLockFreeRingBuffer[int](1024)
	stopMonitor := make(chan struct{})
	var wg sync.WaitGroup

	// 监控goroutine
	wg.Add(1)
	go func() {
		defer wg.Done()
		ticker := time.NewTicker(100 * time.Millisecond)
		defer ticker.Stop()

		for {
			select {
			case <-stopMonitor:
				return
			case <-ticker.C:
				length := rb.Len()
				capacity := rb.Cap()
				usage := float64(length) / float64(capacity) * 100
				fmt.Printf("Buffer使用率: %.1f%% (%d/%d)\n", usage, length, capacity)

				// 告警：使用率过高
				if usage > 80 {
					fmt.Println("⚠️  警告：Buffer使用率超过80%")
				}
			}
		}
	}()

	// 模拟工作负载
	wg.Add(1)
	go func() {
		defer wg.Done()
		for i := 0; i < 500; i++ {
			rb.Push(i)
			time.Sleep(time.Millisecond)
		}
	}()

	wg.Add(1)
	go func() {
		defer wg.Done()
		time.Sleep(50 * time.Millisecond) // 延迟启动消费者
		for i := 0; i < 500; i++ {
			rb.Pop()
			time.Sleep(time.Millisecond)
		}
	}()

	// 等待工作完成
	time.Sleep(600 * time.Millisecond)
	close(stopMonitor)
	wg.Wait()
}

// ExampleHighPerformanceLogging 高性能日志系统示例
type LogEntry struct {
	Timestamp time.Time
	Level     string
	Message   string
}

func ExampleHighPerformanceLogging() {
	// 创建日志缓冲区
	logBuffer := NewLockFreeRingBuffer[LogEntry](8192)
	stopLogging := make(chan struct{})
	var wg sync.WaitGroup

	// 日志写入器（后台线程）
	wg.Add(1)
	go func() {
		defer wg.Done()
		for {
			select {
			case <-stopLogging:
				// 处理剩余日志
				for {
					if entry, ok := logBuffer.Pop(); ok {
						writeToFile(entry)
					} else {
						return
					}
				}
			default:
				if entry, ok := logBuffer.Pop(); ok {
					writeToFile(entry)
				} else {
					time.Sleep(time.Millisecond)
				}
			}
		}
	}()

	// 模拟多个goroutine记录日志
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(workerID int) {
			defer wg.Done()
			for j := 0; j < 100; j++ {
				entry := LogEntry{
					Timestamp: time.Now(),
					Level:     "INFO",
					Message:   fmt.Sprintf("Worker %d: 消息 %d", workerID, j),
				}
				// 非阻塞日志记录
				if !logBuffer.Push(entry) {
					// 缓冲区满，丢弃或采取其他策略
					fmt.Printf("日志缓冲区满，丢弃日志\n")
				}
			}
		}(i)
	}

	// 等待所有工作完成
	wg.Wait()
	close(stopLogging)
	fmt.Println("日志系统已关闭")
}

func writeToFile(entry LogEntry) {
	// 模拟写入文件
	_ = entry
}

// ExampleNetworkPacketBuffer 网络数据包缓冲示例
type Packet struct {
	SourceIP string
	DestIP   string
	Data     []byte
}

func ExampleNetworkPacketBuffer() {
	// 创建数据包缓冲区
	packetBuffer := NewLockFreeRingBuffer[*Packet](4096)
	var wg sync.WaitGroup

	// 网络接收线程
	wg.Add(1)
	go func() {
		defer wg.Done()
		for i := 0; i < 1000; i++ {
			packet := &Packet{
				SourceIP: fmt.Sprintf("192.168.1.%d", i%255),
				DestIP:   "192.168.1.1",
				Data:     make([]byte, 1024),
			}
			for !packetBuffer.Push(packet) {
				runtime.Gosched()
			}
		}
		fmt.Println("接收完成")
	}()

	// 数据包处理线程
	for w := 0; w < 4; w++ {
		wg.Add(1)
		go func(workerID int) {
			defer wg.Done()
			processed := 0
			for processed < 250 { // 每个worker处理250个
				if packet, ok := packetBuffer.Pop(); ok {
					// 处理数据包
					_ = packet
					processed++
				} else {
					runtime.Gosched()
				}
			}
			fmt.Printf("Worker %d 处理完成\n", workerID)
		}(w)
	}

	wg.Wait()
	fmt.Println("所有数据包处理完成")
}
