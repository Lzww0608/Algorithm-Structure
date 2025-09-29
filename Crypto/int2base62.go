package main

import (
	"fmt"
	"math/rand/v2"
	"strings"
)

const base62Chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
const base = uint64(len(base62Chars))

func int2base62(id uint64) string {
	if id == 0 {
		return string(base62Chars[0])
	}

	buf := make([]byte, 0, 12)
	for id > 0 {
		buf = append(buf, base62Chars[id%base])
		id /= base
	}

	for i, j := 0, len(buf)-1; i < j; i, j = i+1, j-1 {
		buf[i], buf[j] = buf[j], buf[i]
	}

	return string(buf)
}

func base622int(str string) uint64 {
	if len(str) == 0 {
		return 0
	}

	buf := []byte(str)

	res := uint64(0)
	for _, b := range buf {
		res = res*base + uint64(strings.IndexByte(base62Chars, b))
	}
	return res
}

func main() {
	for i := 0; i < 1000; i++ {
		id := rand.Uint64()
		str := int2base62(id)
		id1 := base622int(str)
		fmt.Println(id, str, id1)
		if id != id1 {
			fmt.Println("error")
		}
	}
}
