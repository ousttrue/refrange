#include <mpack.h>
#include <gtest/gtest.h>

/// nil:
/// +--------+
/// |  0xc0  |
/// +--------+
TEST(MsgpackTest, nil) 
{
    // packing
    mpack::packer p;
    p.pack_nil();
    auto &buffer=p.packed_buffer;

    // check
    ASSERT_EQ(1, buffer.size());
    EXPECT_EQ(0xc0, buffer[0]);
}

/// false:
/// +--------+
/// |  0xc2  |
/// +--------+
TEST(MsgpackTest, false)
{
    // packing
    mpack::packer p;
    p.pack_bool(false);
    auto &buffer=p.packed_buffer;

    // check
    ASSERT_EQ(1, buffer.size());
    EXPECT_EQ(0xc2, buffer[0]);
}

/// true:
/// +--------+
/// |  0xc3  |
/// +--------+
TEST(MsgpackTest, true)
{
    // packing
    mpack::packer p;
    p.pack_bool(true);
    auto &buffer=p.packed_buffer;

    // check
    ASSERT_EQ(1, buffer.size());
    EXPECT_EQ(0xc3, buffer[0]);
}

/// positive fixnum stores 7-bit positive integer
/// +--------+
/// |0XXXXXXX|
/// +--------+
TEST(MsgpackTest, small_int)
{
    // packing
    mpack::packer p;
    p.pack_int(1);
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(1, buffer.size());
    ASSERT_TRUE(mpack::partial_bit_equal<mpack::PositiveFixint>(buffer[0]));

    // unpack
    mpack::reference_unpacker u(&buffer[0], buffer.size());
    int n=-0;
    u >> n;
    EXPECT_EQ(1, n);
}

/// negative fixnum stores 5-bit negative integer
/// +--------+
/// |111YYYYY|
/// +--------+
TEST(MsgpackTest, small_negative_int)
{
    // packing
    mpack::packer p;
    p.pack_int(-1);
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(1, buffer.size());
    ASSERT_TRUE(mpack::partial_bit_equal<mpack::NegativeFixint>(buffer[0]));

    // unpack
    mpack::reference_unpacker u(&buffer[0], buffer.size());
    int n=0;
    u >> n;
    EXPECT_EQ(-1, n);
}

/// uint 8 stores a 8-bit unsigned integer
/// +--------+--------+
/// |  0xcc  |ZZZZZZZZ|
/// +--------+--------+
TEST(MsgpackTest, uint8)
{
    // packing
    mpack::packer p;
    p.pack_int(128);
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(2, buffer.size());
    EXPECT_EQ(0xcc, buffer[0]);

    // unpack
    mpack::reference_unpacker u(&buffer[0], buffer.size());
    int n=0;
    u >> n;
    EXPECT_EQ(128, n);
}

