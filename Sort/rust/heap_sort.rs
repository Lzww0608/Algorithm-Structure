fn sift_down<T: Ord>(arr: &mut [T], l: usize, r: usize) {
    if l >= r {
        return;
    }

    let (mut fa, mut ch) = (l, l * 2 + 1);
    while ch <= r {
        if ch < r && arr[ch + 1] > arr[ch] {
            ch += 1;
        }

        if arr[fa] >= arr[ch] {
            return;
        }

        arr.swap(fa, ch);
        fa = ch;
        ch = ch * 2 + 1;
    }
}

fn heap_sort<T: Ord>(arr: &mut [T]) {
    let n = arr.len();
    if n <= 1 {
        return;
    }

    for i in (0..=(n - 1 - 1) / 2).rev() {
        sift_down(arr, i, n - 1);
    }

    for i in (1..n).rev() {
        arr.swap(i, 0);
        sift_down(arr, 0, i - 1);
    }
}

fn main() {
    let mut arr = [64, 34, 25, 12, 22, 11, 90];
    heap_sort(&mut arr);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort() {
        let mut arr = [64, 34, 25, 12, 22, 11, 90];
        heap_sort(&mut arr);
        assert_eq!(arr, [11, 12, 22, 25, 34, 64, 90]);
    }

    #[test]
    fn test_empty_array() {
        let mut arr: [i32; 0] = [];
        heap_sort(&mut arr);
        assert_eq!(arr, []);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        heap_sort(&mut arr);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        heap_sort(&mut arr);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_negative_numbers() {
        let mut arr = [-5, 3, -2, 0, 7, -1];
        heap_sort(&mut arr);
        assert_eq!(arr, [-5, -2, -1, 0, 3, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<i32> = (0..100).rev().collect();
        heap_sort(&mut arr);
        let expected: Vec<i32> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_three_elements() {
        let mut arr = [3, 1, 2];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3]);
    }

    #[test]
    fn test_four_elements() {
        let mut arr = [4, 2, 3, 1];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4]);
    }

    #[test]
    fn test_generic_char() {
        let mut arr = ['d', 'a', 'c', 'b', 'e'];
        heap_sort(&mut arr);
        assert_eq!(arr, ['a', 'b', 'c', 'd', 'e']);
    }

    #[test]
    fn test_generic_string() {
        let mut arr = ["zebra", "apple", "banana", "cherry"];
        heap_sort(&mut arr);
        assert_eq!(arr, ["apple", "banana", "cherry", "zebra"]);
    }

    #[test]
    fn test_odd_length() {
        let mut arr = [9, 3, 7, 1, 5];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 3, 5, 7, 9]);
    }

    #[test]
    fn test_even_length() {
        let mut arr = [8, 4, 6, 2];
        heap_sort(&mut arr);
        assert_eq!(arr, [2, 4, 6, 8]);
    }

    #[test]
    fn test_many_duplicates() {
        let mut arr = [5, 2, 5, 2, 5, 2, 5, 2];
        heap_sort(&mut arr);
        assert_eq!(arr, [2, 2, 2, 2, 5, 5, 5, 5]);
    }

    #[test]
    fn test_mixed_values() {
        let mut arr = [100, -100, 50, -50, 0, 25, -25];
        heap_sort(&mut arr);
        assert_eq!(arr, [-100, -50, -25, 0, 25, 50, 100]);
    }

    #[test]
    fn test_power_of_two_size() {
        let mut arr = [8, 4, 2, 7, 1, 3, 6, 5];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5, 6, 7, 8]);
    }

    #[test]
    fn test_seven_elements() {
        let mut arr = [7, 3, 9, 1, 5, 2, 8];
        heap_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 5, 7, 8, 9]);
    }
}