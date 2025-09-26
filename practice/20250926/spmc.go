package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

func producer(ch chan<- string) {
	defer close(ch)

	for i := 1; i <= 5; i++ {
		message := fmt.Sprintf("Message %d", i)
		fmt.Println("Produced: ", message)
		ch <- message
		time.Sleep(time.Millisecond * time.Duration(rand.Intn(1000)))
	}
	fmt.Println("Producer: done")
}

func consumer(ch <-chan string, id int, wg *sync.WaitGroup) {
	defer wg.Done()

	for message := range ch {
		fmt.Printf("Consumer %d: received %s\n", id, message)
		time.Sleep(time.Millisecond * time.Duration(rand.Intn(1000)))
	}

	fmt.Printf("Consumer %d: done\n", id)
}

func broadcast(producerCh <-chan string, consumerChs []chan string) {
	for message := range producerCh {
		fmt.Println("Broadcast: ", message)
		for i, consumerCh := range consumerChs {
			consumerCh <- message
			fmt.Printf("Broadcast: %s to consumer %d\n", message, i)
		}
	}

	fmt.Println("Broadcast: done")
	for _, consumerCh := range consumerChs {
		close(consumerCh)
	}
}

func main() {
	rand.Seed(time.Now().UnixNano())

	producerCh := make(chan string)
	consumerChs := make([]chan string, 3)
	var wg sync.WaitGroup
	for i := range consumerChs {
		ch := make(chan string)
		consumerChs[i] = ch
		wg.Add(1)
		go consumer(ch, i+1, &wg)
	}

	go producer(producerCh)

	go broadcast(producerCh, consumerChs)

	wg.Wait()
	fmt.Println("Main: done")
}
