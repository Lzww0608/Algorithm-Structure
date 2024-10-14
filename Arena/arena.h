
#ifndef STORAGE_ARENA_H
#define STORAGE_ARENA_H

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

class Arena {
private:
	char* AllocateFallBack(size_t bytes);
	char* AllocateNewBlock(size_t block_bytes);

	char* alloc_ptr_;
	size_t alloc_bytes_remaining_;

	std::vector<char*> blocks_;

	std::atomic<size_t> memory_usage_;

public:
	Arena();

	Arena(const Arena&) = delete;
	Arena& operator=(const Arena&) = delete;

	~Arena();

	char* Allocate(size_t bytes);

	char* AllocateAligned(size_t bytes);

	size_t MemoryUsage() const {
		return memory_usage_.load(std::memory_order_relaxed);
	}
};

inline char* Arena::Allocate(size_t bytes) {
	assert(bytes > 0);
	if (bytes <= alloc_bytes_remaining_) {
		char* result = alloc_ptr_;
		alloc_ptr_ += bytes;
		alloc_bytes_remaining_ -= bytes;
		return result;
	}

	return AllocateFallBack(bytes);
}


#endif // !STORAGE_ARENA_H
