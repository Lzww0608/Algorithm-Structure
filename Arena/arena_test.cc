#include "arena.h"
#include <gtest/gtest.h>


// Test allocation fallback when block size is exceeded
TEST(ArenaTest, AllocationFallback) {
	Arena arena;
	size_t initial_memory_usage = arena.MemoryUsage();
	char* ptr = arena.Allocate(5000); // Allocate more than kBlockSize / 4 (which is 1024)
	ASSERT_NE(ptr, nullptr);
	ASSERT_GT(arena.MemoryUsage(), initial_memory_usage + 5000);
}

// Test for aligned allocation
TEST(ArenaTest, AlignedAllocation) {
	Arena arena;
	size_t initial_memory_usage = arena.MemoryUsage();
	char* ptr = arena.AllocateAligned(128);
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr) % (sizeof(void*) > 8 ? sizeof(void*) : 8), 0);
	ASSERT_GT(arena.MemoryUsage(), initial_memory_usage);
}

// Test for multiple aligned allocations
TEST(ArenaTest, MultipleAlignedAllocations) {
	Arena arena;
	char* ptr1 = arena.AllocateAligned(64);
	ASSERT_NE(ptr1, nullptr);
	ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr1) % (sizeof(void*) > 8 ? sizeof(void*) : 8), 0);

	char* ptr2 = arena.AllocateAligned(128);
	ASSERT_NE(ptr2, nullptr);
	ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr2) % (sizeof(void*) > 8 ? sizeof(void*) : 8), 0);

	EXPECT_NE(ptr1, ptr2); // Ensure ptr1 and ptr2 are different memory locations
}

// Test for large allocation
TEST(ArenaTest, LargeAllocation) {
	Arena arena;
	char* ptr = arena.Allocate(10000); // Allocate larger than kBlockSize
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(arena.MemoryUsage(), 10000 + sizeof(char*));
}

// Test for memory usage update
TEST(ArenaTest, MemoryUsage) {
	Arena arena;
	size_t initial_memory_usage = arena.MemoryUsage();

	arena.Allocate(256);
	ASSERT_GT(arena.MemoryUsage(), initial_memory_usage);

	arena.Allocate(512);
	ASSERT_GT(arena.MemoryUsage(), initial_memory_usage + 256);
}

// Test that allocation returns non-overlapping memory
TEST(ArenaTest, NonOverlappingAllocations) {
	Arena arena;
	char* ptr1 = arena.Allocate(128);
	char* ptr2 = arena.Allocate(128);
	ASSERT_NE(ptr1, ptr2);
	ASSERT_EQ(ptr1 + 128, ptr2); // Ensure ptr2 is after ptr1
}

// Test destruction and cleanup
TEST(ArenaTest, DestructorCleansUp) {
	Arena* arena = new Arena();
	arena->Allocate(1024);
	delete arena;
	// If there was a memory leak, sanitizers would report it, no direct ASSERT needed here.
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}