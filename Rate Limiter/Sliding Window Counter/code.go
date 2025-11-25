package main

import (
	"errors"
	"fmt"
	"sync"
	"time"
)

type Slot struct {
	timestamp time.Time
	count     int
}

type SlidingWindowCounter struct {
	mu           sync.Mutex
	limit        int
	windowSize   time.Duration
	slotDuration time.Duration
	slots        []Slot
	numSlots     int
}

func NewSlidingWindowCounter(limit int, windowSize time.Duration, numSlots int) (*SlidingWindowCounter, error) {
	if limit <= 0 {
		return nil, errors.New("limit must be greater than zero")
	}
	if windowSize <= 0 {
		return nil, errors.New("windowSize must be greater than zero")
	}
	if numSlots <= 0 {
		return nil, errors.New("numSlots must be greater than zero")
	}

	return &SlidingWindowCounter{
		limit:        limit,
		windowSize:   windowSize,
		slotDuration: windowSize / time.Duration(numSlots),
		slots:        make([]Slot, numSlots),
		numSlots:     numSlots,
	}, nil
}

func (s *SlidingWindowCounter) Allow() bool {
	s.mu.Lock()
	defer s.mu.Unlock()

	now := time.Now()
	nowNano := now.UnixNano()
	slotDurationNano := s.slotDuration.Nanoseconds()

	idx := (nowNano / slotDurationNano) % int64(s.numSlots)
	currentSlotStart := time.Unix(0, nowNano-(nowNano%slotDurationNano))

	if !s.slots[idx].timestamp.Equal(currentSlotStart) {
		s.slots[idx] = Slot{
			timestamp: currentSlotStart,
			count:     0,
		}
	}

	totalCount := 0
	validWindowStart := now.Add(-s.windowSize)

	for _, v := range s.slots {
		if v.timestamp.After(validWindowStart) {
			totalCount += v.count
		}
	}

	if totalCount >= s.limit {
		return false
	}

	s.slots[idx].count++
	return true
}

func main() {
	// 1秒限流 5次，切分 10 个格子 (每个 100ms)
	limiter, _ := NewSlidingWindowCounter(5, time.Second, 10)

	// 模拟：前 500ms 发送 3 个请求
	for i := 0; i < 3; i++ {
		if limiter.Allow() {
			fmt.Println("Allowed")
		}
	}

	time.Sleep(600 * time.Millisecond)

	// 此时时间过去了 600ms。
	// 在固定窗口算法中，如果跨越了 1s 的边界，计数器会清零，这里可能又允许 5 个。
	// 但在滑动窗口中，前 3 个请求依然在 "最近 1s" 的窗口内。
	// 所以这里应该只能通过 2 个请求 (3 + 2 = 5)。
	for i := 0; i < 5; i++ {
		if limiter.Allow() {
			fmt.Println("New Allowed")
		} else {
			fmt.Println("Blocked")
		}
	}
}
