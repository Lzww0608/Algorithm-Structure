package main

import (
	"context"
	"errors"
	"fmt"
	"sync"
	"time"
)

var (
	ErrRequestExceedsCapacity = errors.New("request tokens exceeds bucket capacity")
	ErrWaitTimeout            = errors.New("wait context canceled or timeout")
)

type TokenBucket struct {
	rate       float64
	capacity   float64
	tokens     float64
	lastRefill time.Time
	mu         sync.Mutex
}

func NewTokenBucket(rate, capacity float64) *TokenBucket {
	if rate <= 0 || capacity <= 0 {
		panic("rate and capacity must be positve!")
	}

	return &TokenBucket{
		rate:       rate,
		capacity:   capacity,
		tokens:     capacity,
		lastRefill: time.Now(),
	}
}

// The caller holds the mutex
func (tb *TokenBucket) Refill(now time.Time) {
	elapses := now.Sub(tb.lastRefill).Seconds()

	generatedTokens := tb.rate * elapses
	if generatedTokens > 0 {
		tb.tokens = min(generatedTokens, tb.capacity)
		tb.lastRefill = now
	}
}

func (tb *TokenBucket) Allow() bool {
	return tb.AllowN(time.Now(), 1)
}

func (tb *TokenBucket) AllowN(now time.Time, n int) bool {
	tb.mu.Lock()
	defer tb.mu.Unlock()

	// lazy refill
	tb.Refill(now)

	if tb.tokens >= float64(n) {
		tb.tokens -= float64(n)
		return true
	}

	return false
}

func (tb *TokenBucket) Wait(ctx context.Context) error {
	return tb.WaitN(ctx, 1)
}

func (tb *TokenBucket) WaitN(ctx context.Context, n int) error {
	select {
	case <-ctx.Done():
		return ctx.Err()
	default:
	}

	if float64(n) > tb.capacity {
		return ErrRequestExceedsCapacity
	}

	tb.mu.Lock()
	now := time.Now()
	tb.Refill(now)

	if tb.tokens >= float64(n) {
		tb.tokens -= float64(n)
		tb.mu.Unlock()
		return nil
	}

	missingTokens := float64(n) - tb.tokens
	waitDuration := time.Duration(missingTokens / tb.rate * float64(time.Second))

	if deadline, ok := ctx.Deadline(); ok {
		if now.Add(waitDuration).After(deadline) {
			return ErrWaitTimeout
		}
	}

	tb.tokens -= float64(n)

	tb.mu.Unlock()

	timer := time.NewTimer(waitDuration)
	defer timer.Stop()

	select {
	case <-timer.C:
		return nil
	case <-ctx.Done():
		return ctx.Err()
	}
}

func main() {
	// 容量为 5，每秒生成 1 个
	tb := NewTokenBucket(1, 5)

	// 1. 瞬间消耗完 5 个
	fmt.Println("Allow 5:", tb.AllowN(time.Now(), 5)) // true

	// 2. 再次消耗，应该失败
	fmt.Println("Allow 1:", tb.Allow()) // false

	// 3. 阻塞等待
	ctx, cancel := context.WithTimeout(context.Background(), 3*time.Second)
	defer cancel()

	fmt.Println("Waiting for 1 token...")
	start := time.Now()
	err := tb.Wait(ctx)
	fmt.Printf("Wait result: %v, took: %v\n", err, time.Since(start))
	// 预期：大约等待 1 秒后成功
}
