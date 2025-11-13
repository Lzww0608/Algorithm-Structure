package main

import "testing"

func siftDown(arr []int, l, r int) {
	fa, ch := l, l*2+1
	for ch <= r {
		if ch+1 <= r && arr[ch+1] > arr[ch] {
			ch++
		}
		if arr[fa] < arr[ch] {
			arr[fa], arr[ch] = arr[ch], arr[fa]
			fa, ch = ch, ch*2+1
		} else {
			break
		}
	}
}

func heapSort(arr []int) []int {
	n := len(arr)
	for i := n/2 - 1; i >= 0; i-- {
		siftDown(arr, i, n-1)
	}

	for i := n - 1; i > 0; i-- {
		arr[0], arr[i] = arr[i], arr[0]
		siftDown(arr, 0, i-1)
	}

	return arr
}

func TestHeapSort(t *testing.T) {
	testCases := []struct {
		name     string
		input    []int
		expected []int
	}{
		{
			name:     "空数组",
			input:    []int{},
			expected: []int{},
		},
		{
			name:     "单个元素",
			input:    []int{5},
			expected: []int{5},
		},
		{
			name:     "已排序数组",
			input:    []int{1, 2, 3, 4, 5},
			expected: []int{1, 2, 3, 4, 5},
		},
		{
			name:     "逆序数组",
			input:    []int{5, 4, 3, 2, 1},
			expected: []int{1, 2, 3, 4, 5},
		},
		{
			name:     "随机数组",
			input:    []int{3, 1, 4, 1, 5, 9, 2, 6},
			expected: []int{1, 1, 2, 3, 4, 5, 6, 9},
		},
		{
			name:     "含重复元素",
			input:    []int{2, 2, 1, 3, 3, 3},
			expected: []int{1, 2, 2, 3, 3, 3},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			// 复制输入数组以避免修改原始测试数据
			input := make([]int, len(tc.input))
			copy(input, tc.input)

			result := heapSort(input)

			if !slicesEqual(result, tc.expected) {
				t.Errorf("heapSort(%v) = %v, 期望 %v", tc.input, result, tc.expected)
			}
		})
	}
}

// 辅助函数：比较两个切片是否相等
func slicesEqual(a, b []int) bool {
	if len(a) != len(b) {
		return false
	}
	for i := range a {
		if a[i] != b[i] {
			return false
		}
	}
	return true
}
