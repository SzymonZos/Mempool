#include "Config.hpp"

#include <gtest/gtest.h>

namespace mp::ut {

TEST(MempoolConfig, GetTotalSizeTest1) {
    MempoolConfig config{{1024, 1024}, {1024, 2048}};
    auto size = GetTotalSize(config);
    EXPECT_EQ(size.bytes, 1024 * 1024 + 1024 * 2048);
}

TEST(MempoolConfig, GetTotalSizeTest2) {
    MempoolConfig config{{1024, 1024}, {1024, 512}};
    auto size = GetTotalSize(config);
    EXPECT_EQ(size.bytes, 1024 * 1024 + 1024 * 512);
}

} // namespace mp::ut
