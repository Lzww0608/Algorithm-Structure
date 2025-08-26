/*
@Author: Lzww
@LastEditTime: 2025-8-26 19:53:13
@Description: Ring Buffer
@Language: Go 1.23.4
*/

package safeudp

// RingBuffer is a generic circular buffer implementation
// It uses a fixed-size array with head and tail pointers to efficiently
// manage elements in a FIFO manner with automatic growth when needed
type RingBuffer[T any] struct {
	buffer []T // underlying array to store elements
	head   int // index of the first element
	tail   int // index where the next element will be inserted
}

// Empty returns true if the ring buffer contains no elements
func (rb *RingBuffer[T]) Empty() bool {
	return rb.head == rb.tail
}

// Full returns true if the ring buffer is at maximum capacity
// Note: we reserve one slot to distinguish between empty and full states
func (rb *RingBuffer[T]) Full() bool {
	return (rb.tail+1)%len(rb.buffer) == rb.head
}

// MaxLen returns the maximum number of elements the buffer can hold
// This is one less than the buffer size due to the empty/full distinction
func (rb *RingBuffer[T]) MaxLen() int {
	return len(rb.buffer) - 1
}

// Push adds a new element to the tail of the ring buffer
// If the buffer is full, it will automatically grow to accommodate the new element
// Returns true on successful insertion
func (rb *RingBuffer[T]) Push(value T) bool {
	if rb.Full() {
		rb.grow()
	}
	rb.buffer[rb.tail] = value
	rb.tail = (rb.tail + 1) % len(rb.buffer)
	return true
}

// Pop removes and returns the element at the head of the ring buffer
// Returns the element and true if successful, or zero value and false if empty
func (rb *RingBuffer[T]) Pop() (T, bool) {
	var zero T
	if rb.Empty() {
		return zero, false
	}
	value := rb.buffer[rb.head]
	rb.buffer[rb.head] = zero // clear the slot to prevent memory leaks
	rb.head = (rb.head + 1) % len(rb.buffer)
	return value, true
}

// Peek returns a pointer to the element at the head without removing it
// Returns pointer to element and true if successful, or nil and false if empty
func (rb *RingBuffer[T]) Peek() (*T, bool) {
	if rb.Empty() {
		return nil, false
	}
	return &rb.buffer[rb.head], true
}

// Len returns the current number of elements in the ring buffer
func (rb *RingBuffer[T]) Len() int {
	if rb.tail >= rb.head {
		return rb.tail - rb.head
	}
	return len(rb.buffer) - rb.head + rb.tail
}

// ForEach iterates through all elements in the ring buffer from head to tail
// The provided function receives a pointer to each element
// If the function returns false, iteration stops early
func (rb *RingBuffer[T]) ForEach(fn func(*T) bool) {
	if rb.Empty() {
		return
	}

	if rb.head < rb.tail {
		// Simple case: no wraparound
		for i := rb.head; i < rb.tail; i++ {
			if !fn(&rb.buffer[i]) {
				return
			}
		}
	} else {
		// Wraparound case: iterate from head to end, then from start to tail
		for i := rb.head; i < len(rb.buffer); i++ {
			if !fn(&rb.buffer[i]) {
				return
			}
		}
		for i := 0; i < rb.tail; i++ {
			if !fn(&rb.buffer[i]) {
				return
			}
		}
	}
}

// ForEachReverse iterates through all elements in reverse order (tail to head)
// The provided function receives a pointer to each element
// If the function returns false, iteration stops early
func (rb *RingBuffer[T]) ForEachReverse(fn func(*T) bool) {
	if rb.Empty() {
		return
	}

	if rb.head < rb.tail {
		// Simple case: no wraparound, iterate backwards
		for i := rb.tail - 1; i >= rb.head; i-- {
			if !fn(&rb.buffer[i]) {
				return
			}
		}
	} else {
		// Wraparound case: iterate from tail-1 to start, then from end to head
		for i := rb.tail - 1; i >= 0; i-- {
			if !fn(&rb.buffer[i]) {
				return
			}
		}
		for i := len(rb.buffer) - 1; i >= rb.head; i-- {
			if !fn(&rb.buffer[i]) {
				return
			}
		}
	}
}

// Discard removes the first n elements from the ring buffer
// Returns the actual number of elements discarded (may be less than n if buffer has fewer elements)
func (rb *RingBuffer[T]) Discard(n int) int {
	if n <= 0 {
		return 0
	}

	if rb.Empty() {
		return 0
	}

	if n >= rb.Len() {
		n = rb.Len()
	}

	var zero T
	for range n {
		rb.buffer[rb.head] = zero // clear the slot to prevent memory leaks
		rb.head = (rb.head + 1) % len(rb.buffer)
	}
	return n
}

// grow increases the capacity of the ring buffer when it becomes full
// The new capacity is calculated as current length + 10% (minimum 1 additional slot)
// All existing elements are copied to the new buffer in order
func (rb *RingBuffer[T]) grow() {
	currentLen := rb.Len()
	newCapacity := currentLen + (currentLen+9)/10 + 1 // grow by ~10% plus extra slot
	if newCapacity < currentLen+2 {
		newCapacity = currentLen + 2 // ensure at least 2 extra slots
	}
	newBuffer := make([]T, newCapacity+1) // +1 for empty/full distinction

	// Copy elements using index instead of append
	index := 0
	rb.ForEach(func(item *T) bool {
		newBuffer[index] = *item
		index++
		return true
	})

	rb.buffer = newBuffer
	rb.head = 0
	rb.tail = currentLen
}
