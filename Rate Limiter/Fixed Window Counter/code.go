package main

import (
	"errors"
	"fmt"
	"sync"
	"time"
)

type FixedWindowCounter struct {
	limit      int
	windowSize time.Duration

	mu           sync.Mutex
	currentCount int
	lastRest     time.Time
}

func NewFixedWindowCounter(limit int, windowSize time.Duration) (*FixedWindowCounter, error) {
	if limit <= 0 {
		return nil, errors.New("limit must be greater than zero")
	}
	if windowSize <= 0 {
		return nil, errors.New("windowSize must be greater than zero")
	}

	return &FixedWindowCounter{
		limit:        limit,
		windowSize:   windowSize,
		currentCount: 0,
		lastRest:     time.Now(),
	}, nil
}

func (f *FixedWindowCounter) Allow() bool {
	f.mu.Lock()
	defer f.mu.Unlock()

	if time.Since(f.lastRest) > f.windowSize {
		f.currentCount = 0
		f.lastRest = time.Now()
	}

	if f.currentCount < f.limit {
		f.currentCount++
		return true
	}

	return false
}

func (f *FixedWindowCounter) Reset() {
	f.mu.Lock()
	defer f.mu.Unlock()

	f.currentCount = 0
	f.lastRest = time.Now()
}

func (f *FixedWindowCounter) Stat() (int, time.Time) {
	f.mu.Lock()
	defer f.mu.Unlock()

	return f.currentCount, f.lastRest
}

func main() {
	// 限制每 1 秒只能通过 3 个请求
	limiter, _ := NewFixedWindowCounter(3, time.Second)

	// 模拟连续请求
	for i := 0; i < 10; i++ {
		if limiter.Allow() {
			fmt.Printf("[%d] Passed\n", i+1)
		} else {
			fmt.Printf("[%d] Limited\n", i+1)
		}
		// 快速发送，模拟瞬间流量
		time.Sleep(100 * time.Millisecond)
	}

	// 等待窗口过期
	fmt.Println("Waiting for window reset...")
	time.Sleep(1 * time.Second)

	if limiter.Allow() {
		fmt.Println("New request Passed after reset")
	}
}
