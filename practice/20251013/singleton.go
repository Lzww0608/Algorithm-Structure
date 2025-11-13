package main

import (
	"fmt"
	"sync"
)

type Singleton struct {
}

var once sync.Once
var instance *Singleton

func GetInstance() *Singleton {
	once.Do(func() {
		instance = &Singleton{}
		fmt.Println("Singleton created")
	})

	return instance
}
