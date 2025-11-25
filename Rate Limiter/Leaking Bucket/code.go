package main

import (
	"errors"
	"fmt"
	"sync"
	"sync/atomic"
	"time"
)

var (
	ErrBucketFull   = errors.New("bucket full, requests dropped")
	ErrBucketClosed = errors.New("bucket closed")
)

type Handler func(item any)

type LeakingBucket struct {
	queue    chan any
	interval time.Duration
	handler  Handler

	running  uint32
	stopChan chan struct{}
	wg       sync.WaitGroup
}

func NewLeakingBucket(rate float64, capacity int, handler Handler) *LeakingBucket {
	if rate <= 0 || capacity <= 0 {
		panic("rate and capacity must be positive")
	}
	interval := time.Duration(float64(time.Second) / rate)

	lb := &LeakingBucket{
		queue:    make(chan any, capacity),
		interval: interval,
		handler:  handler,
		running:  1,
		stopChan: make(chan struct{}),
		wg:       sync.WaitGroup{},
	}

	lb.wg.Add(1)
	go lb.Start()
	return lb
}

func (lb *LeakingBucket) Push(item any) error {
	if atomic.LoadUint32(&lb.running) == 0 {
		return ErrBucketClosed
	}

	select {
	case lb.queue <- item:
		return nil
	default:
		return ErrBucketFull
	}
}

func (lb *LeakingBucket) Start() {
	defer lb.wg.Done()

	ticker := time.NewTicker(lb.interval)
	defer ticker.Stop()

	for {
		select {
		case <-lb.stopChan:
			lb.drainingRemain()
			return
		case <-ticker.C:
			select {
			case item := <-lb.queue:
				go lb.safeHandle(item)
			default:
			}
		}
	}
}

func (lb *LeakingBucket) safeHandle(item any) {
	defer func() {
		if r := recover(); r != nil {
			fmt.Println("Recovered in leaking bucket", r)
		}
	}()

	lb.handler(item)
}

func (lb *LeakingBucket) drainingRemain() {
Loop:
	for {
		select {
		case item := <-lb.queue:
			go lb.safeHandle(item)
		default:
			break Loop
		}
	}
}

func (lb *LeakingBucket) Close() {
	if !atomic.CompareAndSwapUint32(&lb.running, 1, 0) {
		return
	}
	close(lb.stopChan)

	lb.wg.Wait()
}

func main() {
	// 创建一个漏桶：每秒处理 2 个请求，桶容量 5
	// 模拟一个慢速消费者
	lb := NewLeakingBucket(2, 5, func(item any) {
		id := item.(int)
		fmt.Printf("[%s] Processing item %d\n", time.Now().Format("15:04:05.000"), id)
		// 模拟处理耗时
		time.Sleep(50 * time.Millisecond)
	})

	// 模拟突发流量：瞬间发送 10 个请求
	for i := 1; i <= 10; i++ {
		err := lb.Push(i)
		if err != nil {
			fmt.Printf("Item %d dropped: %v\n", i, err)
		} else {
			fmt.Printf("Item %d pushed\n", i)
		}
	}

	// 等待一会观察输出
	time.Sleep(3 * time.Second)

	// 优雅关闭
	fmt.Println("Closing bucket...")
	lb.Close()
	fmt.Println("Bucket closed.")
}
