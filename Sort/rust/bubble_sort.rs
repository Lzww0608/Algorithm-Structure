
fn bubble_sort(arr: &mut [i32]) {
    let n = arr.len();
    let mut swapped = false;
    if n <= 1 {
        return;
    }

    for i in 0..n - 1 {
        for j in 0..n - i - 1 {
            if arr[j] > arr[j + 1] {
                arr.swap(j, j + 1);
                swapped = true;
            }
        }
        if !swapped {
            break;
        }
    }
}

fn main() {
    let mut arr = [64, 34, 25, 12, 22, 11, 90];
    bubble_sort(&mut arr);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort() {
        let mut arr = [64, 34, 25, 12, 22, 11, 90];
        bubble_sort(&mut arr);
        assert_eq!(arr, [11, 12, 22, 25, 34, 64, 90]);
    }

    #[test]
    fn test_empty_array() {
        let mut arr: [i32; 0] = [];
        bubble_sort(&mut arr);
        assert_eq!(arr, []);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        bubble_sort(&mut arr);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        bubble_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];
        bubble_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        bubble_sort(&mut arr);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        bubble_sort(&mut arr);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        bubble_sort(&mut arr);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_negative_numbers() {
        let mut arr = [-5, 3, -2, 0, 7, -1];
        bubble_sort(&mut arr);
        assert_eq!(arr, [-5, -2, -1, 0, 3, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<i32> = (0..100).rev().collect();
        bubble_sort(&mut arr);
        let expected: Vec<i32> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        bubble_sort(&mut arr);
        assert_eq!(arr, [1, 2]);
    }
}

