fn merge_sort<T: PartialOrd + Copy + Default> (arr: &mut[T], l: usize, r: usize) {
    if l >= r {
        return;
    }

    let mid = l + ((r - l) >> 1);
    merge_sort(arr, l, mid);
    merge_sort(arr, mid + 1, r);

    let mut i = l;
    let mut j = mid + 1;
    let mut k = 0;
    let mut tmp: Vec<T> = vec![T::default(); r - l + 1];
    
    while i <= mid || j <= r {
        if j > r || (i <= mid && arr[i] <= arr[j]) {
            tmp[k] = arr[i];
            i += 1;
        } else {
            tmp[k] = arr[j];
            j += 1;
        }
        k += 1;
    }
    
    arr[l..=r].copy_from_slice(&tmp);
}

fn main() {
    let mut arr = [64, 34, 25, 12, 22, 11, 90];
    let len = arr.len();
    merge_sort(&mut arr, 0, len - 1);
    println!("Sorted array: {:?}", arr);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_sort() {
        let mut arr = [64, 34, 25, 12, 22, 11, 90];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [11, 12, 22, 25, 34, 64, 90]);
    }

    #[test]
    fn test_single_element() {
        let mut arr = [42];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [42]);
    }

    #[test]
    fn test_two_elements() {
        let mut arr = [2, 1];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_two_elements_sorted() {
        let mut arr = [1, 2];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2]);
    }

    #[test]
    fn test_already_sorted() {
        let mut arr = [1, 2, 3, 4, 5];  
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_reverse_sorted() {
        let mut arr = [5, 4, 3, 2, 1];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_duplicate_elements() {
        let mut arr = [3, 1, 4, 1, 5, 9, 2, 6, 5];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 1, 2, 3, 4, 5, 5, 6, 9]);
    }

    #[test]
    fn test_all_same_elements() {
        let mut arr = [7, 7, 7, 7, 7];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [7, 7, 7, 7, 7]);
    }

    #[test]
    fn test_negative_numbers() {
        let mut arr = [-5, 3, -2, 0, 7, -1];
        let len = arr.len();
            merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [-5, -2, -1, 0, 3, 7]);
    }

    #[test]
    fn test_large_array() {
        let mut arr: Vec<i32> = (0..100).rev().collect();
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        let expected: Vec<i32> = (0..100).collect();
        assert_eq!(arr, expected);
    }

    #[test]
    fn test_three_elements() {
        let mut arr = [3, 1, 2];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3]);
    }

    #[test]
    fn test_four_elements() {
        let mut arr = [4, 2, 3, 1];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 2, 3, 4]);
    }

    #[test]
    fn test_generic_float() {
        let mut arr = [3.14, 1.41, 2.71, 0.5, 4.2];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [0.5, 1.41, 2.71, 3.14, 4.2]);
    }

    #[test]
    fn test_generic_char() {
        let mut arr = ['d', 'a', 'c', 'b', 'e'];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, ['a', 'b', 'c', 'd', 'e']);
    }

    #[test]
    fn test_odd_length() {
        let mut arr = [9, 3, 7, 1, 5];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [1, 3, 5, 7, 9]);
    }

    #[test]
    fn test_even_length() {
        let mut arr = [8, 4, 6, 2];
        let len = arr.len();
        merge_sort(&mut arr, 0, len - 1);
        assert_eq!(arr, [2, 4, 6, 8]);
    }

    #[test]
    fn test_partial_sort() {
        let mut arr = [5, 2, 8, 1, 9, 3, 7];
        // 只排序索引 1-5 的部分（2, 8, 1, 9, 3）
        merge_sort(&mut arr, 1, 5);
        assert_eq!(arr, [5, 1, 2, 3, 8, 9, 7]);
    }
}
