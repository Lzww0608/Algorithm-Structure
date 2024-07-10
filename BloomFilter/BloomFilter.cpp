// To be optimized
#include <iostream>
#include <vector>
#include <functional>

uint32_t murmurhash3(const char* key, size_t len, uint32_t seed) {
	uint32_t c1 = 0xcc9e2d51;
	uint32_t c2 = 0x1b873593;
	uint32_t r1 = 15;
	uint32_t r2 = 13;
	uint32_t m = 5;
	uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t* blocks = (const uint32_t*)(key);
	int i;
	for (i = 0; i < nblocks; i++) {
		uint32_t k = blocks[i];

		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = (hash << r2) | (hash >> (32 - r2));
		hash = hash * m + n;
	}

	const uint8_t* tail = (const uint8_t*)(key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];
		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	};

	hash ^= len;

	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;

	return hash;
}

class BloomFilter {
public:
	BloomFilter(size_t size, size_t numHashFunctions)
		: bitArray(size, 0), numHashFunctions(numHashFunctions) { }

	void add(const std::string &item) {
		for (size_t i = 0; i < numHashFunctions; ++i) {
			size_t hash = hashFunction(item, i);
			bitArray[hash % bitArray.size()] = 1;
		}
	}

	bool contains(const std::string &item) {
		for (size_t i = 0; i < numHashFunctions; ++i) {
			size_t hash = hashFunction(item, i);
			if (bitArray[hash % bitArray.size()] != 1) {
				return false;
			}
		}

		return true;
	}


private:
	size_t hashFunction(const std::string &item, size_t seed) const {
		return murmurhash3(item.c_str(), item.size(), seed);
	}

	std::vector<uint8_t> bitArray;
	size_t numHashFunctions;
};


int main() {
	size_t size = 1000;
	size_t numHashFunctions = 7;

	BloomFilter bloomFilter(size, numHashFunctions);

	bloomFilter.add("hello");
	bloomFilter.add("world");

	std::cout << "Contains 'hello': " << bloomFilter.contains("hello") << std::endl;
	std::cout << "Contains 'world': " << bloomFilter.contains("world") << std::endl;
	std::cout << "Contains 'bloom': " << bloomFilter.contains("bloom") << std::endl;

	return 0;
}