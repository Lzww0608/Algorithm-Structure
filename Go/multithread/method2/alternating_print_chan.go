package main

import (
	"fmt"
	"runtime"
	"sync"
	"time"
)

const MAX_NUMBER = 1_000_000

func printEven(evenChan, oddChan chan int, wg *sync.WaitGroup) {
	defer wg.Done()
	for num := range evenChan {
		if num > MAX_NUMBER {
			close(oddChan) // 只关闭通道，不再发送
			break
		}
		fmt.Println("Even Thread: ", num)
		oddChan <- num + 1
	}
}

func printOdd(evenChan, oddChan chan int, wg *sync.WaitGroup) {
	defer wg.Done()
	for num := range oddChan {
		if num > MAX_NUMBER {
			close(evenChan) // 只关闭通道，不再发送
			break
		}
		fmt.Println("Odd Thread: ", num)
		evenChan <- num + 1
	}
}

// 1k Time used: 0.013996 seconds
// 100w Time used: 31.711371 seconds
func main() {
	start := time.Now()

	runtime.GOMAXPROCS(2)

	evenChan := make(chan int)
	oddChan := make(chan int)

	var wg sync.WaitGroup
	wg.Add(2)

	go printEven(evenChan, oddChan, &wg)
	go printOdd(evenChan, oddChan, &wg)

	// 启动打印，从1开始
	oddChan <- 1

	wg.Wait()

	end := time.Now()
	duration := end.Sub(start)
	fmt.Printf("Time used: %.6f seconds\n", duration.Seconds())
}
