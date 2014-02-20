#include <refrange/msgpack.h>
#include <gtest/gtest.h>

TEST(MsgpackTest, bits) 
{
    EXPECT_EQ(0, refrange::msgpack::positive_fixint_tag::bits);
	EXPECT_EQ(0x80, refrange::msgpack::positive_fixint_tag::mask);
}

