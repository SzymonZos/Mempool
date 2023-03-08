#include "Mempool.hpp"

#include <numeric>

#include <gtest/gtest.h>

namespace mp::ut {

namespace {
std::int64_t GetPtrDiff(void* lhs, void* rhs) {
    return static_cast<std::byte*>(lhs) - static_cast<std::byte*>(rhs);
}

template<typename T>
T GetSecondToLastElement(const std::vector<T>& vec) {
    return vec.rbegin()[1];
}
} // namespace

TEST(MempoolInitialization, ConstructsPoolWithValidAlignment) {
    EXPECT_NO_THROW({ Mempool mempool({}, 64); });
}

TEST(MempoolInitialization, ThrowsOnTooSmallAlignment) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_THROW({ Mempool mempool(config, 0); }, MempoolExcetption);
}

TEST(MempoolInitialization, ThrowsOnTooChunkSizeNotBeinMultipleOfAlignment) {
    MempoolConfig config{{1024, 1024}, {1024, 255}};
    EXPECT_THROW({ Mempool mempool(config, 64); }, MempoolExcetption);
}

TEST(MempoolInitialization, ConstructsPoolWithValidSize) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({ Mempool mempool(config, 64); });
}

TEST(MempoolInitialization, ThrowsOnSizeBiggerThanTotalSize) {
    MempoolConfig config{{1024, 1024}, {1024, 2048}};
    EXPECT_THROW({ Mempool mempool(config, 64); }, MempoolExcetption);
}

TEST(MempoolAllocation, AllocsValidSize) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        EXPECT_NE(mempool.alloc(512), nullptr);
    });
}

TEST(MempoolAllocation, AllocatedMemoryCanBeAssigned) {
    MempoolConfig config{{1024, 1024}, {1024, 64}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        auto ptr = static_cast<std::int32_t*>(mempool.alloc(64));
        std::iota(ptr, ptr + 16, 0);
        for (std::int32_t i{}; i < 16; i++) {
            EXPECT_EQ(ptr[i], i);
        }
    });
}

TEST(MempoolAllocation, ReturnsNullptrOnSizeBiggerThanTheBiggestChunk) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        EXPECT_EQ(mempool.alloc(2048), nullptr);
    });
}

TEST(MempoolAllocation, ReturnsNullptrOnNoChunksLeftInSubPool) {
    MempoolConfig config{{1024, 1024}, {2, 2048}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        EXPECT_NE(mempool.alloc(1500), nullptr);
        EXPECT_NE(mempool.alloc(1700), nullptr);
        EXPECT_EQ(mempool.alloc(1500), nullptr);
    });
}

TEST(MempoolAllocation, ReturnsProperlyIncrementedPointer) {
    MempoolConfig config{{32, 1024}, {256, 512}};
    EXPECT_NO_THROW({
        std::vector<void*> ptrs;
        ptrs.reserve(32);
        Mempool mempool(config, 64);

        ptrs.push_back(mempool.alloc(666));
        ASSERT_NE(ptrs.back(), nullptr);

        ptrs.push_back(mempool.alloc(777));
        EXPECT_EQ(GetPtrDiff(ptrs.back(), ptrs.front()), 1024);

        ptrs.push_back(mempool.alloc(888));
        EXPECT_EQ(GetPtrDiff(ptrs.back(), GetSecondToLastElement(ptrs)), 1024);

        ptrs.push_back(mempool.alloc(420));
        ASSERT_NE(ptrs.back(), nullptr);

        ptrs.push_back(mempool.alloc(200));
        EXPECT_EQ(GetPtrDiff(ptrs.back(), GetSecondToLastElement(ptrs)), 512);

        EXPECT_EQ(GetPtrDiff(ptrs.front(), GetSecondToLastElement(ptrs)), 256 * 512);
    });
}

TEST(MempoolAllocation, DenselyAllocatedSubPoolsDoNotInterleave) {
    MempoolConfig config{{2, 1024}, {2, 512}};
    EXPECT_NO_THROW({
        std::vector<void*> ptrs;
        ptrs.reserve(32);
        Mempool mempool(config, 64);

        ptrs.push_back(mempool.alloc(666));
        ASSERT_NE(ptrs.back(), nullptr);

        ptrs.push_back(mempool.alloc(777));
        EXPECT_EQ(GetPtrDiff(ptrs.back(), ptrs.front()), 1024);

        EXPECT_EQ(mempool.alloc(777), nullptr);

        ptrs.push_back(mempool.alloc(420));
        ASSERT_NE(ptrs.back(), nullptr);

        ptrs.push_back(mempool.alloc(200));
        EXPECT_EQ(GetPtrDiff(ptrs.back(), GetSecondToLastElement(ptrs)), 512);

        EXPECT_EQ(mempool.alloc(42), nullptr);

        EXPECT_EQ(GetPtrDiff(ptrs.front(), GetSecondToLastElement(ptrs)), 2 * 512);
        EXPECT_EQ(GetPtrDiff(ptrs[0], ptrs[3]), 512);
    });
}

TEST(MempoolFree, FreesValidPointer) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        auto ptr = mempool.alloc(512);
        EXPECT_NE(mempool.free(ptr), nullptr);
    });
}

TEST(MempoolFree, ReturnsNullptrOnNotAllocatedAddress) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        auto ptr = mempool.alloc(512);
        auto invalid_ptr = static_cast<std::byte*>(ptr) - 10;
        EXPECT_EQ(mempool.free(invalid_ptr), nullptr);
    });
}

TEST(MempoolFree, ReturnsNullptrOnNotAlignedAddress) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        auto ptr = mempool.alloc(512);
        auto invalid_ptr = static_cast<std::byte*>(ptr) + 10;
        EXPECT_EQ(mempool.free(invalid_ptr), nullptr);
    });
}

TEST(MempoolAllocAndFree, ProperlyKeepsTrackOfAllocatedIndices) {
    MempoolConfig config{{4, 1024}, {4, 512}};
    EXPECT_NO_THROW({
        std::vector<void*> ptrs;
        ptrs.reserve(32);
        Mempool mempool(config, 64);

        ptrs.push_back(mempool.alloc(666));
        ASSERT_NE(ptrs.back(), nullptr);

        ptrs.push_back(mempool.alloc(777));
        ptrs.push_back(mempool.alloc(888));
        ptrs.push_back(mempool.free(ptrs[1]));
        ptrs.push_back(mempool.alloc(999));
        EXPECT_EQ(ptrs.back(), GetSecondToLastElement(ptrs));
    });
}
} // namespace mp::ut
