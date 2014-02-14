#include <mpack.h>
#include <gtest/gtest.h>

TEST(MsgpackTest, bits) 
{
    EXPECT_EQ(0, mpack::positive_fixint::bits);
    EXPECT_EQ(0x80, mpack::positive_fixint::mask);
}

