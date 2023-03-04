#include "Mempool.hpp"

#include <gtest/gtest.h>

namespace mp::ut {

TEST(MempoolInitialization, ConstructsPoolWithValidAlignment) {
    EXPECT_NO_THROW({ Mempool mempool({}, 64); });
}

TEST(MempoolInitialization, ThrowsOnTooSmallAlignment) {
    MempoolConfig config{{1024, 1024}, {1024, 2048}};
    EXPECT_THROW({ Mempool mempool(config, 0); }, MempoolExcetption);
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

TEST(MempoolAllocation, ReturnsNullptrOnSizeBiggerThanTheBiggestChunk) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        EXPECT_EQ(mempool.alloc(2048), nullptr);
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

TEST(MempoolFree, ReturnsNullptrOnSizeBiggerThanTheBiggestChunk) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    EXPECT_NO_THROW({
        Mempool mempool(config, 64);
        auto ptr = mempool.alloc(512);
        auto invalid_ptr = static_cast<std::byte*>(ptr) - 10;
        EXPECT_EQ(mempool.free(invalid_ptr), nullptr);
    });
}
} // namespace mp::ut
