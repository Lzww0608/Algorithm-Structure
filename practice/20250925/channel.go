package main

import (
	"fmt"
	"time"
)

func worker(id int, tasks <-chan int, done <-chan struct{}) {
	fmt.Printf("Worker %d startes\n", id)
	for {
		select {
		case task, ok := <-tasks:
			if !ok {
				fmt.Printf("Worker %d: task channel closed\n", id)
				return
			}
			fmt.Printf("Worker %d: processing task %d\n", id, task)
			time.Sleep(time.Microsecond * 100)
		case <-done:
			fmt.Printf("Worker %d: received shutdown signal\n", id)
			fmt.Printf("Worker %d: exiting\n", id)
			return
		}
	}
}

func main() {
	ch := make(chan int, 5)
	done := make(chan struct{})

	go worker(1, ch, done)
	for i := 1; i <= 3; i++ {
		ch <- i
	}

	time.Sleep(time.Second * 1)
	fmt.Println("Sending shutdown signal")
	close(done)

	time.Sleep(time.Second * 1)
	fmt.Println("Main program exiting")
}
