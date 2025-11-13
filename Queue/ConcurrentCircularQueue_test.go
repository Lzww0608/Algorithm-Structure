package main

import (
	"sort"
	"sync"
	"testing"
	"time"
)

type ConcurrentCircularQueue[T any] struct {
	buffer   []T
	head     int
	tail     int
	size     int
	capacity int
	mu       *sync.Mutex
	notFull  *sync.Cond
	notEmpty *sync.Cond
}

func NewConcurrentCircularQueue[T any](capacity int) *ConcurrentCircularQueue[T] {
	if capacity <= 0 {
		panic("capacity must be greater than 0")
	}
	m := &sync.Mutex{}
	return &ConcurrentCircularQueue[T]{
		buffer:   make([]T, capacity),
		head:     0,
		tail:     0,
		size:     0,
		capacity: capacity,
		mu:       m,
		notFull:  sync.NewCond(m),
		notEmpty: sync.NewCond(m),
	}
}

func (q *ConcurrentCircularQueue[T]) Push(value T) {
	q.mu.Lock()
	defer q.mu.Unlock()

	// to avoid false wakeup, we use a loop to check the condition
	for q.size == q.capacity {
		q.notFull.Wait()
	}
	q.buffer[q.tail] = value
	q.tail = (q.tail + 1) % q.capacity
	q.size++
	q.notEmpty.Signal()
}

func (q *ConcurrentCircularQueue[T]) Pop() T {
	q.mu.Lock()
	defer q.mu.Unlock()
	for q.size == 0 {
		q.notEmpty.Wait()
	}
	value := q.buffer[q.head]
	q.head = (q.head + 1) % q.capacity
	q.size--
	q.notFull.Signal()
	return value
}

func (q *ConcurrentCircularQueue[T]) Size() int {
	q.mu.Lock()
	defer q.mu.Unlock()
	return q.size
}

func (q *ConcurrentCircularQueue[T]) Empty() bool {
	q.mu.Lock()
	defer q.mu.Unlock()
	return q.size == 0
}

func (q *ConcurrentCircularQueue[T]) Full() bool {
	q.mu.Lock()
	defer q.mu.Unlock()
	return q.size == q.capacity
}

func TestConcurrentCircularQueue(t *testing.T) {
	// Test basic functionality
	t.Run("BasicOperations", func(t *testing.T) {
		q := NewConcurrentCircularQueue[int](3)

		// Test empty queue
		if !q.Empty() {
			t.Error("New queue should be empty")
		}
		if q.Full() {
			t.Error("New queue should not be full")
		}
		if q.Size() != 0 {
			t.Errorf("Expected size 0, got %d", q.Size())
		}

		// Test push operations
		q.Push(1)
		q.Push(2)
		q.Push(3)

		if q.Empty() {
			t.Error("Queue should not be empty after pushes")
		}
		if !q.Full() {
			t.Error("Queue should be full after 3 pushes")
		}
		if q.Size() != 3 {
			t.Errorf("Expected size 3, got %d", q.Size())
		}

		// Test pop operations
		val1 := q.Pop()
		if val1 != 1 {
			t.Errorf("Expected 1, got %d", val1)
		}

		val2 := q.Pop()
		if val2 != 2 {
			t.Errorf("Expected 2, got %d", val2)
		}

		val3 := q.Pop()
		if val3 != 3 {
			t.Errorf("Expected 3, got %d", val3)
		}

		if !q.Empty() {
			t.Error("Queue should be empty after all pops")
		}
	})

	// Test concurrent operations
	t.Run("ConcurrentOperations", func(t *testing.T) {
		q := NewConcurrentCircularQueue[int](100)
		const numProducers = 5
		const numConsumers = 3
		const itemsPerProducer = 20

		var wg sync.WaitGroup
		produced := make([]int, 0, numProducers*itemsPerProducer)
		consumed := make([]int, 0, numProducers*itemsPerProducer)
		var producedMu, consumedMu sync.Mutex

		// Start producers
		for i := 0; i < numProducers; i++ {
			wg.Add(1)
			go func(producerID int) {
				defer wg.Done()
				for j := 0; j < itemsPerProducer; j++ {
					value := producerID*itemsPerProducer + j
					q.Push(value)

					producedMu.Lock()
					produced = append(produced, value)
					producedMu.Unlock()
				}
			}(i)
		}

		// Start consumers
		for i := 0; i < numConsumers; i++ {
			wg.Add(1)
			go func() {
				defer wg.Done()
				itemsToConsume := (numProducers * itemsPerProducer) / numConsumers
				for j := 0; j < itemsToConsume; j++ {
					value := q.Pop()

					consumedMu.Lock()
					consumed = append(consumed, value)
					consumedMu.Unlock()
				}
			}()
		}

		wg.Wait()

		// Handle remaining items if any
		for !q.Empty() {
			value := q.Pop()
			consumedMu.Lock()
			consumed = append(consumed, value)
			consumedMu.Unlock()
		}

		// Verify all items were produced and consumed
		if len(produced) != numProducers*itemsPerProducer {
			t.Errorf("Expected %d produced items, got %d", numProducers*itemsPerProducer, len(produced))
		}
		if len(consumed) != numProducers*itemsPerProducer {
			t.Errorf("Expected %d consumed items, got %d", numProducers*itemsPerProducer, len(consumed))
		}

		// Verify all produced items were consumed
		sort.Ints(produced)
		sort.Ints(consumed)
		for i := 0; i < len(produced); i++ {
			if produced[i] != consumed[i] {
				t.Errorf("Mismatch at index %d: produced %d, consumed %d", i, produced[i], consumed[i])
			}
		}
	})

	// Test blocking behavior
	t.Run("BlockingBehavior", func(t *testing.T) {
		q := NewConcurrentCircularQueue[int](2)

		// Test that Push blocks when queue is full
		q.Push(1)
		q.Push(2)

		done := make(chan bool, 1)
		go func() {
			q.Push(3) // This should block
			done <- true
		}()

		// Give some time for the goroutine to block
		time.Sleep(100 * time.Millisecond)
		select {
		case <-done:
			t.Error("Push should have blocked when queue is full")
		default:
			// Expected behavior
		}

		// Pop an item to unblock the Push
		val := q.Pop()
		if val != 1 {
			t.Errorf("Expected 1, got %d", val)
		}

		// Now the Push should complete
		select {
		case <-done:
			// Expected behavior
		case <-time.After(1 * time.Second):
			t.Error("Push should have unblocked after Pop")
		}

		// Test that Pop blocks when queue is empty
		q.Pop() // Remove second item
		q.Pop() // Remove third item (queue now empty)

		done2 := make(chan bool, 1)
		go func() {
			q.Pop() // This should block
			done2 <- true
		}()

		// Give some time for the goroutine to block
		time.Sleep(100 * time.Millisecond)
		select {
		case <-done2:
			t.Error("Pop should have blocked when queue is empty")
		default:
			// Expected behavior
		}

		// Push an item to unblock the Pop
		q.Push(4)

		// Now the Pop should complete
		select {
		case <-done2:
			// Expected behavior
		case <-time.After(1 * time.Second):
			t.Error("Pop should have unblocked after Push")
		}
	})
}
