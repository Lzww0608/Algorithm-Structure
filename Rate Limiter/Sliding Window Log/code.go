package main

import (
	"errors"
	"fmt"
	"sort"
	"sync"
	"time"
)

type SlidingWindowLog struct {
	mu     sync.Mutex
	limit  int           // 窗口内的最大请求数
	window time.Duration // 窗口大小
	logs   []int64       // 存储请求时间戳的日志(UnixNano)
}

func NewSlidingWindowLog(limit int, window time.Duration) (*SlidingWindowLog, error) {
	if limit <= 0 {
		return nil, errors.New("limit must be greater than zero")
	}
	if window <= 0 {
		return nil, errors.New("window must be greater than zero")
	}

	return &SlidingWindowLog{
		limit:  limit,
		window: window,
		logs:   make([]int64, 0, limit),
	}, nil
}

func (s *SlidingWindowLog) Allow() bool {
	s.mu.Lock()
	defer s.mu.Unlock()

	now := time.Now().UnixNano()
	boundary := now - int64(s.window)

	p := sort.Search(len(s.logs), func(i int) bool { return s.logs[i] > boundary })

	if p >= 0 {
		s.logs = s.logs[p:]
	}

	if len(s.logs) >= s.limit {
		return false
	}

	s.logs = append(s.logs, now)
	return true
}

func (s *SlidingWindowLog) Stat() (currentCount, remaining int) {
	s.mu.Lock()
	defer s.mu.Unlock()

	return len(s.logs), s.limit - len(s.logs)
}

func main() {
	// 限制 1秒内最多 3 个请求
	limiter, _ := NewSlidingWindowLog(3, time.Second)

	// 模拟请求
	for i := 0; i < 10; i++ {
		allowed := limiter.Allow()
		status := "Allowed"
		if !allowed {
			status = "Rejected"
		}
		fmt.Println(time.Now().Format("15:04:05.000"), "Request", i, status)

		// 快速发送前5个，后面慢速发送
		if i < 4 {
			time.Sleep(100 * time.Millisecond)
		} else {
			time.Sleep(400 * time.Millisecond)
		}
	}
}
