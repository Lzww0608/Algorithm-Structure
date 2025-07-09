package main

import (
	"fmt"
	"math"
)

const TARGET float64 = 24
const EPS float64 = 1e-6

func judgePoint24(cards []int) bool {
	initialCards := []float64{float64(cards[0]), float64(cards[1]), float64(cards[2]), float64(cards[3])}
	initialExprs := []string{fmt.Sprintf("%.0f", initialCards[0]), fmt.Sprintf("%.0f", initialCards[1]), fmt.Sprintf("%.0f", initialCards[2]), fmt.Sprintf("%.0f", initialCards[3])}
	return dfs(initialCards, initialExprs)
}

func dfs(cards []float64, exprs []string) bool {
	n := len(cards)
	if n == 1 {
		if math.Abs(cards[0]-TARGET) < EPS {
			fmt.Println(exprs[0])
			return true
		}
		return false
	}
	for i := 0; i < n; i++ {
		for j := i + 1; j < n; j++ {
			nxt := make([]float64, 0, n-1)
			nxtExprs := make([]string, 0, n-1)
			for k := 0; k < n; k++ {
				if k != i && k != j {
					nxt = append(nxt, cards[k])
					nxtExprs = append(nxtExprs, exprs[k])
				}
			}
			for _, r := range solve(cards[i], cards[j], exprs[i], exprs[j]) {
				nxt = append(nxt, r.val)
				nxtExprs = append(nxtExprs, r.expr)
				if dfs(nxt, nxtExprs) {
					return true
				}
				nxt = nxt[:len(nxt)-1]
				nxtExprs = nxtExprs[:len(nxtExprs)-1]
			}

		}
	}
	return false
}

type result struct {
	val  float64
	expr string
}

func solve(a, b float64, exprA, exprB string) (results []result) {
	results = append(results, result{a + b, fmt.Sprintf("(%s + %s)", exprA, exprB)})
	results = append(results, result{a - b, fmt.Sprintf("(%s - %s)", exprA, exprB)})
	results = append(results, result{b - a, fmt.Sprintf("(%s - %s)", exprB, exprA)})
	results = append(results, result{a * b, fmt.Sprintf("(%s * %s)", exprA, exprB)})
	if math.Abs(b) > EPS {
		results = append(results, result{a / b, fmt.Sprintf("(%s / %s)", exprA, exprB)})
	}
	if math.Abs(a) > EPS {
		results = append(results, result{b / a, fmt.Sprintf("(%s / %s)", exprB, exprA)})
	}
	return results
}

func main() {
	cards := []int{4, 1, 8, 7}
	fmt.Println(judgePoint24(cards)) // Output: false
}
