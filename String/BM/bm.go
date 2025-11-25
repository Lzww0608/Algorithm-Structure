package bm

const ASCII_SIZE = 256

func getBadCharTable(pattern string) []int {

	table := make([]int, ASCII_SIZE)
	for i := range table {
		table[i] = -1
	}

	// populate the table with the last occurrence of each character in the pattern.
	for i := range pattern {
		table[pattern[i]] = i
	}

	return table
}

func getGoodSuffixTable(pattern string) []int {
	n := len(pattern)
	if n == 0 {
		return nil
	}

	// get suffix array
	// suffix[i] is the length of the longest common suffix
	// between pattern[0:i + 1] and the whole pattern.
	suffix := make([]int, n)
	suffix[n-1] = n

	g, f := n-1, n-1
	for i := n - 2; i >= 0; i-- {
		if i > g && suffix[i+n-1-f] < i-g {
			suffix[i] = suffix[i+n-1-f]
		} else {
			if i < g {
				g = i
			}

			f = i
			for g >= 0 && pattern[g] == pattern[g+n-1-f] {
				g--
			}

			suffix[i] = f - g
		}
	}

	gs := make([]int, n)
	for i := range n {
		gs[i] = n
	}

	// Case 2: A prefix of the pattern matches a suffix of the good suffix.
	// j is the start index of the prefix.
	j := 0
	for i := n - 1; i >= 0; i-- {
		// If suffix[i] == i + 1, then pattern[:i + 1] is a suffix of the pattern.
		if suffix[i] == i+1 {
			for ; j < n-1-i; j++ {
				if gs[j] == n {
					gs[j] = n - 1 - i
				}
			}
		}
	}

	// Case 1: Another occurence of the good suffix exists in the pattern.
	// This will overwrite the default shifts.
	for i := range n - 1 {
		len_suffix := suffix[i]

		// Mismatch occurs at index (n - 1 - len_suffix)
		// The shift value is (n - 1 - i)
		gs[n-1-len_suffix] = n - 1 - i
	}

	return gs
}

func bm(text string, pattern string) (ans []int) {
	n, m := len(text), len(pattern)
	if n == 0 || m == 0 || m > n {
		return nil
	}

	bc := getBadCharTable(pattern)
	gs := getGoodSuffixTable(pattern)

	for i := 0; i <= n-m; i++ {
		j := m - 1
		for j >= 0 && pattern[j] == text[i+j] {
			j--
		}

		if j < 0 {
			ans = append(ans, i)
			shift := gs[0]
			if shift == 0 {
				shift = 1
			}
			i += shift
			continue
		}

		badShift := j - bc[text[i+j]]
		if badShift < 1 {
			badShift = 1
		}

		goodShift := gs[j]
		if goodShift < 1 {
			goodShift = 1
		}

		i += max(badShift, goodShift)
	}

	return
}
