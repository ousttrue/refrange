#include <mpack.h>
#include <gtest/gtest.h>

TEST(MsgpackTest, bits) 
{
    EXPECT_EQ(0, mpack::msgpack::positive_fixint_tag::bits);
	EXPECT_EQ(0x80, mpack::msgpack::positive_fixint_tag::mask);
}

