// 计数排序只适用于 usize 类型的非负整数
fn counting_sort(arr: &mut [usize]) {
    let n = arr.len();
    if n <= 1 {
        return;
    }

    let mx = *arr.iter().max().unwrap();
    let mut b = vec![0; n];
    let mut cnt = vec![0; mx + 1];

    for &x in arr.iter() {
        cnt[x] += 1;
    }
    
    for i in 1..=mx {
        cnt[i] += cnt[i - 1];
    }

    for i in (0..n).rev() {
        cnt[arr[i]] -= 1;
        b[cnt[arr[i]]] = arr[i];
    }

    arr.copy_from_slice(&b);
}

fn main() {
    let mut arr = [64, 34, 25, 12, 22, 11, 90];
    counting_sort(&mut arr);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort() {
        let mut arr = [64, 34, 25, 12, 22, 11, 90];
        counting_sort(&mut arr);
        assert_eq!(arr, [11, 12, 22, 25, 34, 64, 90]);
    }

    #[test]
    fn test_empty_array() {
        let mut arr: [usize; 0] = [];
        counting_sort(&mut arr);
        assert_eq!(arr, []);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        counting_sort(&mut arr);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        counting_sort(&mut arr);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_with_zeros() {
        let mut arr = [5, 0, 3, 0, 7, 0, 1];
        counting_sort(&mut arr);
        assert_eq!(arr, [0, 0, 0, 1, 3, 5, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<usize> = (0..100).rev().collect();
        counting_sort(&mut arr);
        let expected: Vec<usize> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_three_elements() {
        let mut arr = [3, 1, 2];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3]);
    }

    #[test]
    fn test_four_elements() {
        let mut arr = [4, 2, 3, 1];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4]);
    }

    #[test]
    fn test_odd_length() {
        let mut arr = [9, 3, 7, 1, 5];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 3, 5, 7, 9]);
    }

    #[test]
    fn test_even_length() {
        let mut arr = [8, 4, 6, 2];
        counting_sort(&mut arr);
        assert_eq!(arr, [2, 4, 6, 8]);
    }

    #[test]
    fn test_many_duplicates() {
        let mut arr = [5, 2, 5, 2, 5, 2, 5, 2];
        counting_sort(&mut arr);
        assert_eq!(arr, [2, 2, 2, 2, 5, 5, 5, 5]);
    }

    #[test]
    fn test_small_range() {
        let mut arr = [3, 1, 2, 3, 1, 2, 3, 1, 2];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 1, 1, 2, 2, 2, 3, 3, 3]);
    }

    #[test]
    fn test_large_values() {
        let mut arr = [100, 50, 75, 25, 90, 10];
        counting_sort(&mut arr);
        assert_eq!(arr, [10, 25, 50, 75, 90, 100]);
    }

    #[test]
    fn test_sequential() {
        let mut arr = [5, 6, 7, 8, 9];
        counting_sort(&mut arr);
        assert_eq!(arr, [5, 6, 7, 8, 9]);
    }

    #[test]
    fn test_stability() {
        // 测试稳定性：相同元素的相对顺序应该保持
        let mut arr = [3, 1, 3, 2, 3, 1];
        counting_sort(&mut arr);
        assert_eq!(arr, [1, 1, 2, 3, 3, 3]);
    }
}
