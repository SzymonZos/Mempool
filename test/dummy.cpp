#include "dummy.hpp"

#include <gtest/gtest.h>

namespace mp::ut {

TEST(DummyTest, ReturnsZero) {
    EXPECT_EQ(dummy(), 0);
}
}// namespace mp::ut
