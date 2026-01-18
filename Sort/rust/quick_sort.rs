fn quick_sort<T: Ord + Copy>(arr: &mut [T], l: usize, r: usize) {
    if l >= r {
        return;
    }

    let mut pivot = arr[l];
    let mid = l + (r - l) / 2;
    if arr[mid] <= arr[l] && arr[mid] >= arr[r] || arr[mid] >= arr[l] && arr[mid] <= arr[r] {
        pivot = arr[mid];
    } else if arr[r] <= arr[l] && arr[r] >= arr[mid] || arr[r] >= arr[l] && arr[r] <= arr[mid] {
        pivot = arr[r];
    }

    let mut i = l;  
    let mut j = l;
    let mut k = r + 1;
    
    while i < k {
        if arr[i] > pivot {
            k -= 1;
            arr.swap(i, k);
        } else if arr[i] < pivot {
            arr.swap(i, j);
            i += 1;
            j += 1;
        } else {
            i += 1;
        }
    }

    if j > 0 && j > l {
        quick_sort(arr, l, j - 1);
    }
    if k <= r {
        quick_sort(arr, k, r);
    }
}

fn main() {
    let mut arr = [64, 34, 25, 12, 22, 11, 90];
    let len = arr.len();
    quick_sort(&mut arr, 0, len - 1);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort() {
        let mut arr = [64, 34, 25, 12, 22, 11, 90];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [11, 12, 22, 25, 34, 64, 90]);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        quick_sort(&mut arr, 0, 0);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_negative_numbers() {
        let mut arr = [-5, 3, -2, 0, 7, -1];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [-5, -2, -1, 0, 3, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<i32> = (0..100).rev().collect();
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        let expected: Vec<i32> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_three_elements() {
        let mut arr = [3, 1, 2];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3]);
    }

    #[test]
    fn test_four_elements() {
        let mut arr = [4, 2, 3, 1];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3, 4]);
    }

    #[test]
    fn test_generic_char() {
        let mut arr = ['d', 'a', 'c', 'b', 'e'];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, ['a', 'b', 'c', 'd', 'e']);
    }

    #[test]
    fn test_odd_length() {
        let mut arr = [9, 3, 7, 1, 5];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 3, 5, 7, 9]);
    }

    #[test]
    fn test_even_length() {
        let mut arr = [8, 4, 6, 2];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [2, 4, 6, 8]);
    }

    #[test]
    fn test_many_duplicates() {
        let mut arr = [5, 2, 5, 2, 5, 2, 5, 2];
        let len = arr.len();
        quick_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [2, 2, 2, 2, 5, 5, 5, 5]);
    }

    #[test]
    fn test_partial_sort() {
        let mut arr = [5, 2, 8, 1, 9, 3, 7];
        // 只排序索引 1-5 的部分（2, 8, 1, 9, 3）
        quick_sort(&mut arr, 1, 5);
        assert_eq!(arr, [5, 1, 2, 3, 8, 9, 7]);
    }
}
