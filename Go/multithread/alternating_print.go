package main

import (
	"fmt"
	"runtime"
	"sync"
	"time"
)

var (
	currentNumber int  = 1
	turnEven      bool = false
	mutex         sync.Mutex
	cond          *sync.Cond
)

const MAX_NUMBER = 1_000_000

func printEven() {
	for currentNumber <= MAX_NUMBER {
		mutex.Lock()
		for !turnEven && currentNumber <= MAX_NUMBER {
			cond.Wait()
		}

		if currentNumber > MAX_NUMBER {
			mutex.Unlock()
			break
		}

		if currentNumber%2 == 0 {
			fmt.Println("Even Thread: ", currentNumber)
			currentNumber++
			turnEven = false
			cond.Signal()
		}

		mutex.Unlock()
	}
}

func printOdd() {
	for currentNumber <= MAX_NUMBER {
		mutex.Lock()
		for turnEven && currentNumber <= MAX_NUMBER {
			cond.Wait()
		}

		if currentNumber > MAX_NUMBER {
			mutex.Unlock()
			break
		}

		if currentNumber%2 == 1 {
			fmt.Println("Odd Thread: ", currentNumber)
			currentNumber++
			turnEven = true
			cond.Signal()
		}

		mutex.Unlock()
	}
}

// 1k Time used: 0.012983 seconds
// 100w Time used: 29.991467 seconds
func main() {
	start := time.Now()

	runtime.GOMAXPROCS(2)

	mutex = sync.Mutex{}
	cond = sync.NewCond(&mutex)

	var wg sync.WaitGroup
	wg.Add(2)

	go func() {
		defer wg.Done()
		printOdd()
	}()

	go func() {
		defer wg.Done()
		printEven()
	}()

	wg.Wait()

	end := time.Now()
	duration := end.Sub(start)
	fmt.Printf("Time used: %.6f seconds\n", duration.Seconds())
}
