#ifndef BloomFilter_LevelDB_H
#define BloomFilter_LevelDB_H

#include <stddef.h>

void InitializeBloomFilter(int n, int bits_per_key, char** dst, size_t* dst_size);

void UpdateBloomFilter(const char* keys[], int n, char* bloom_filter, size_t bloom_filter_size);

bool keyMayMatch(const char** keys, const char* bloom_filter, size_t bloom_filter_size);

#endif // !BloomFilter_LevelDB_H
