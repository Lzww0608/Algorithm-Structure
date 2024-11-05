#include <iostream>
#include <smmintrin.h>
#include <cstring>
#include <vector>

#include "smhasher/MurmurHash3.h"
#include "BloomFilter_LevelDB.h"

constexpr uint32_t seed = 42;

void InitializeBloomFilter(int n, int bits_per_key, char** dst, size_t* dst_size) {
    // k = ln(2) * (m / n)
    // 0.69 is approximately ln(2)
    size_t k = static_cast<size_t>(bits_per_key * 0.69);  
    if (k < 1) k = 1;
    if (k > 30) k = 30;

    // Calculate the size of the bit array in bits.
    size_t bits = n * bits_per_key;
    if (bits < 64) bits = 64; 
    size_t bytes = (bits + 7) / 8; 

    // Allocate memory for the filter (including space for 'k')
    std::vector<char> filter(bytes, 0);
    filter.push_back(static_cast<char>(k)); 

    // Return the result as a new string
    *dst_size = filter.size();
    *dst = new char[*dst_size];
    std::memcpy(*dst, filter.data(), *dst_size);
}


void UpdateBloomFilter(const char* keys[], int n, char* bloom_filter, size_t bloom_filter_size) {
    if (bloom_filter_size < 2) return;  

    char* array = bloom_filter;
    size_t bits = (bloom_filter_size - 1) * 8;  
    size_t k = array[bloom_filter_size - 1];  

    if (k > 30) return;  

    for (int i = 0; i < n; ++i) {
        uint32_t h = 0;
        MurmurHash3_x86_32(keys[i], strlen(keys[i]), seed, &h);
        const uint32_t delta = (h >> 17) | (h << 15);  // Double hashing step
        for (size_t j = 0; j < k; ++j) {
            const uint32_t bitpos = h % bits;
            array[bitpos / 8] |= (1 << (bitpos % 8));  // Set the appropriate bit.
            h += delta;  // Move to the next hash position.
        }
    }
}

// Check if a key might be in the bloom filter.
bool KeyMayMatch(const char* key, const char* bloom_filter, size_t bloom_filter_size) {
    if (bloom_filter_size < 2) return false;

    const char* array = bloom_filter;
    const size_t bits = (bloom_filter_size - 1) * 8;  
    const size_t k = array[bloom_filter_size - 1];  

    if (k > 30) return true;  

    uint32_t h = 0;
    MurmurHash3_x86_32(key, strlen(key), seed, &h);
    const uint32_t delta = (h >> 17) | (h << 15);  
    for (size_t j = 0; j < k; ++j) {
        const uint32_t bitpos = h % bits;
        if ((array[bitpos / 8] & (1 << (bitpos % 8))) == 0) {
            return false;  
        }
        h += delta;  
    }
    return true; 
}





int main() {
    char* bloom_filter = nullptr;
    size_t bloom_filter_size = 0;
    int bits_per_key = 10;
    int num_initial_keys = 0;  
    InitializeBloomFilter(num_initial_keys, bits_per_key, &bloom_filter, &bloom_filter_size);

    const char* keys_to_insert_1[] = {"apple", "banana", "cherry"};
    UpdateBloomFilter(keys_to_insert_1, 3, bloom_filter, bloom_filter_size);

    const char* keys_to_insert_2[] = {"date", "elderberry", "fig"};
    UpdateBloomFilter(keys_to_insert_2, 3, bloom_filter, bloom_filter_size);

    const char* test_key = "apple";
    bool result = KeyMayMatch(test_key, bloom_filter, bloom_filter_size);
    std::cout << "Key '" << test_key << "' might be in the filter: " << result << std::endl;

    delete[] bloom_filter;

    return 0;
}