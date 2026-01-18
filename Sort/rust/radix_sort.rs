fn counting_sort_by_digit(arr: &mut [usize], exp: usize) {
    let n = arr.len();
    let mut output = vec![0; n];
    let mut count = vec![0; 10];

    for &num in arr.iter() {
        let digit = (num / exp) % 10;
        count[digit] += 1;
    }

    for i in 1..10 {
        count[i] += count[i - 1];
    }

    for i in (0..n).rev() {
        let digit = (arr[i] / exp) % 10;
        count[digit] -= 1;
        output[count[digit]] = arr[i];
    }

    arr.copy_from_slice(&output);
}

fn radix_sort(arr: &mut [usize]) {
    let n = arr.len();
    if n <= 1 {
        return;
    }

    let max_val = *arr.iter().max().unwrap();

    let mut exp = 1;
    while max_val / exp > 0 {
        counting_sort_by_digit(arr, exp);
        exp *= 10;
    }
}

fn main() {
    let mut arr = [170, 45, 75, 90, 802, 24, 2, 66];
    println!("Original array: {:?}", arr);
    radix_sort(&mut arr);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort() {
        let mut arr = [170, 45, 75, 90, 802, 24, 2, 66];
        radix_sort(&mut arr);
        assert_eq!(arr, [2, 24, 45, 66, 75, 90, 170, 802]);
    }

    #[test]
    fn test_empty_array() {
        let mut arr: [usize; 0] = [];
        radix_sort(&mut arr);
        assert_eq!(arr, []);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        radix_sort(&mut arr);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        radix_sort(&mut arr);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_with_zeros() {
        let mut arr = [5, 0, 3, 0, 7, 0, 1];
        radix_sort(&mut arr);
        assert_eq!(arr, [0, 0, 0, 1, 3, 5, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<usize> = (0..100).rev().collect();
        radix_sort(&mut arr);
        let expected: Vec<usize> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_three_elements() {
        let mut arr = [3, 1, 2];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3]);
    }

    #[test]
    fn test_four_elements() {
        let mut arr = [4, 2, 3, 1];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4]);
    }

    #[test]
    fn test_single_digit() {
        let mut arr = [9, 3, 7, 1, 5];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 3, 5, 7, 9]);
    }

    #[test]
    fn test_two_digits() {
        let mut arr = [89, 34, 67, 12, 45];
        radix_sort(&mut arr);
        assert_eq!(arr, [12, 34, 45, 67, 89]);
    }

    #[test]
    fn test_three_digits() {
        let mut arr = [329, 457, 657, 839, 436, 720, 355];
        radix_sort(&mut arr);
        assert_eq!(arr, [329, 355, 436, 457, 657, 720, 839]);
    }

    #[test]
    fn test_mixed_digit_lengths() {
        let mut arr = [1, 10, 100, 1000, 5, 50, 500];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 5, 10, 50, 100, 500, 1000]);
    }

    #[test]
    fn test_many_duplicates() {
        let mut arr = [5, 2, 5, 2, 5, 2, 5, 2];
        radix_sort(&mut arr);
        assert_eq!(arr, [2, 2, 2, 2, 5, 5, 5, 5]);
    }

    #[test]
    fn test_large_numbers() {
        let mut arr = [9999, 1111, 5555, 3333, 7777];
        radix_sort(&mut arr);
        assert_eq!(arr, [1111, 3333, 5555, 7777, 9999]);
    }

    #[test]
    fn test_stability() {
        // 测试稳定性
        let mut arr = [121, 432, 564, 23, 1, 45, 788];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 23, 45, 121, 432, 564, 788]);
    }

    #[test]
    fn test_power_of_ten() {
        let mut arr = [1000, 100, 10, 1, 10000];
        radix_sort(&mut arr);
        assert_eq!(arr, [1, 10, 100, 1000, 10000]);
    }

    #[test]
    fn test_consecutive_numbers() {
        let mut arr = [15, 14, 13, 12, 11];
        radix_sort(&mut arr);
        assert_eq!(arr, [11, 12, 13, 14, 15]);
    }
}
