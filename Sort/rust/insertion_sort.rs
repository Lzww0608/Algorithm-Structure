fn insertion_sort<T: PartialOrd + Copy>(arr: &mut [T]) {
    let n = arr.len();
    if n <= 1 {
        return;
    }

    for i in 1..n {
        let mut j = i;
        let cur = arr[i];
        while j > 0 && arr[j - 1] > cur {
            arr[j] = arr[j - 1];
            j = j - 1;
        }

        arr[j] = cur;
    }
}

fn main() {
    let mut arr = [64, 34, 25, 12, 22, 11, 90];
    insertion_sort(&mut arr);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort_i32() {
        let mut arr = [64, 34, 25, 12, 22, 11, 90];
        insertion_sort(&mut arr);
        assert_eq!(arr, [11, 12, 22, 25, 34, 64, 90]);
    }

    #[test]
    fn test_empty_array() {
        let mut arr: [i32; 0] = [];
        insertion_sort(&mut arr);
        assert_eq!(arr, []);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        insertion_sort(&mut arr);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        insertion_sort(&mut arr);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_negative_numbers() {
        let mut arr = [-5, 3, -2, 0, 7, -1];
        insertion_sort(&mut arr);
        assert_eq!(arr, [-5, -2, -1, 0, 3, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<i32> = (0..100).rev().collect();
        insertion_sort(&mut arr);
        let expected: Vec<i32> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_generic_float() {
        let mut arr = [3.14, 1.41, 2.71, 0.5, 4.2];
        insertion_sort(&mut arr);
        assert_eq!(arr, [0.5, 1.41, 2.71, 3.14, 4.2]);
    }

    #[test]
    fn test_generic_char() {
        let mut arr = ['d', 'a', 'c', 'b', 'e'];
        insertion_sort(&mut arr);
        assert_eq!(arr, ['a', 'b', 'c', 'd', 'e']);
    }

    #[test]
    fn test_three_elements() {
        let mut arr = [3, 1, 2];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3]);
    }

    #[test]
    fn test_insert_at_beginning() {
        let mut arr = [10, 1, 2, 3, 4];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 10]);
    }

    #[test]
    fn test_insert_in_middle() {
        let mut arr = [1, 3, 2, 4, 5];
        insertion_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }
}
