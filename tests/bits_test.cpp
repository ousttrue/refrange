#include <mpack.h>
#include <gtest/gtest.h>

TEST(MsgpackTest, bits) 
{
    EXPECT_EQ(0, mpack::PositiveFixint::bits);
    EXPECT_EQ(0x80, mpack::PositiveFixint::mask);
}


