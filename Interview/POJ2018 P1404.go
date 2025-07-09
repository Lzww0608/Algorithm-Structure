package main

import "fmt"

func main() {
	var n, m int
	fmt.Scan(&n, &m)
	a := make([]int, n)
	pre := make([]int, n+1)
	mx := 0
	for i := range a {
		fmt.Scan(&a[i])
		a[i] *= 10000
		mx = max(mx, a[i])
	}

	l, r := 0, mx
	for l < r {
		mid := l + ((r - l) >> 1)
		mn := 0
		f := false
		for i := range a {
			pre[i+1] = pre[i] + a[i] - mid
			if i >= m-1 {
				mn = min(mn, pre[i-m+1])
				if pre[i+1] > mn {
					f = true
					break
				}
			}
		}
		if f {
			l = mid + 1
		} else {
			r = mid
		}
	}
	fmt.Println(l / 10)
}
