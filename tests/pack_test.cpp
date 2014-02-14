#include <mpack.h>
#include <gtest/gtest.h>

/// nil:
/// +--------+
/// |  0xc0  |
/// +--------+
TEST(MsgpackTest, nil) 
{
    // packing
    mpack::vector_packer p;
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
    mpack::vector_packer p;
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
    mpack::vector_packer p;
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
    mpack::vector_packer p;
    p << 1;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(1, buffer.size());
    ASSERT_TRUE(mpack::partial_bit_equal<mpack::positive_fixint>(buffer[0]));

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
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
    mpack::vector_packer p;
    p << -1;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(1, buffer.size());
    ASSERT_TRUE(mpack::partial_bit_equal<mpack::negative_fixint>(buffer[0]));

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
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
    mpack::vector_packer p;
    p << 128;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(2, buffer.size());
    EXPECT_EQ(0xcc, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    int n=0;
    u >> n;
    EXPECT_EQ(128, n);
}

/// uint 16 stores a 16-bit big-endian unsigned integer
/// +--------+--------+--------+
/// |  0xcd  |ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+
TEST(MsgpackTest, uint16)
{
    // packing
    mpack::vector_packer p;
    p << 256;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(3, buffer.size());
    EXPECT_EQ(0xcd, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    int n=0;
    u >> n;
    EXPECT_EQ(256, n);
}

/// uint 32 stores a 32-bit big-endian unsigned integer
/// +--------+--------+--------+--------+--------+
/// |  0xce  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ
/// +--------+--------+--------+--------+--------+
TEST(MsgpackTest, uint32)
{
    {
        int value=65536;

        // packing
        mpack::vector_packer p;
        p << value;
        auto &buffer=p.packed_buffer;
        ASSERT_FALSE(buffer.empty());

        // check
        ASSERT_EQ(5, buffer.size());
        EXPECT_EQ(0xce, buffer[0]);

        // unpack
        auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
        int n=0;
        u >> n;
        EXPECT_EQ(value, n);
    }

    {
        unsigned int value=4294967295;

        // packing
        mpack::vector_packer p;
        p << value;
        auto &buffer=p.packed_buffer;
        ASSERT_FALSE(buffer.empty());

        // check
        ASSERT_EQ(5, buffer.size());
        EXPECT_EQ(0xce, buffer[0]);

        // unpack
        auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
        int n=0;
        u >> n;
        EXPECT_EQ(value, n);
    }
}

/// uint 64 stores a 64-bit big-endian unsigned integer
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
/// |  0xcf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
TEST(MsgpackTest, uint64)
{
    unsigned long long value=4294967296;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(9, buffer.size());
    EXPECT_EQ(0xcf, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    unsigned long long n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// int 8 stores a 8-bit signed integer
/// +--------+--------+
/// |  0xd0  |ZZZZZZZZ|
/// +--------+--------+
TEST(MsgpackTest, int8)
{
    char value=-32;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(2, buffer.size());
    EXPECT_EQ(0xd0, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    char n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// int 16 stores a 16-bit big-endian signed integer
/// +--------+--------+--------+
/// |  0xd1  |ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+
TEST(MsgpackTest, int16)
{
    int value=-256;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(3, buffer.size());
    EXPECT_EQ(0xd1, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    int n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// int 32 stores a 32-bit big-endian signed integer
/// +--------+--------+--------+--------+--------+
/// |  0xd2  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+--------+--------+
TEST(MsgpackTest, int32)
{
    int value=-65535;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(5, buffer.size());
    EXPECT_EQ(0xd2, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    int n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// int 64 stores a 64-bit big-endian signed integer
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
/// |  0xd3  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
TEST(MsgpackTest, int64)
{
    long long value=-4294967296;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(9, buffer.size());
    EXPECT_EQ(0xd3, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    long long n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// float 32 stores a floating point number in IEEE 754 single precision floating point number format:
/// +--------+--------+--------+--------+--------+
/// |  0xca  |XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX
/// +--------+--------+--------+--------+--------+
TEST(MsgpackTest, float32)
{
    float value=1.5f;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(5, buffer.size());
    EXPECT_EQ(0xca, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    float n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// float 64 stores a floating point number in IEEE 754 double precision floating point number format:
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
/// |  0xcb  |YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
TEST(MsgpackTest, float64)
{
    double value=1.111111111111111111111111111111111111111111111111;

    // packing
    mpack::vector_packer p;
    p << value;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(9, buffer.size());
    EXPECT_EQ(0xcb, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    double n=0;
    u >> n;
    EXPECT_EQ(value, n);
}

/// fixstr stores a byte array whose length is upto 31 bytes:
/// +--------+========+
/// |101XXXXX|  data  |
/// +--------+========+
TEST(MsgpackTest, fixstr)
{
    auto str="abc";

    // packing
    mpack::vector_packer p;
    p << str;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(1+3, buffer.size());
    ASSERT_TRUE(mpack::partial_bit_equal<mpack::fixstr>(buffer[0]));

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    std::string out;
    u >> out;

    EXPECT_EQ(out, str);
}

/// str 8 stores a byte array whose length is upto (2^8)-1 bytes:
/// +--------+--------+========+
/// |  0xd9  |YYYYYYYY|  data  |
/// +--------+--------+========+
TEST(MsgpackTest, str8)
{
    auto str=
        "0123456789"
        "0123456789"
        "0123456789"
        "01"
        ;

    // packing
    mpack::vector_packer p;
    p << str;
    auto &buffer=p.packed_buffer;
    ASSERT_FALSE(buffer.empty());

    // check
    ASSERT_EQ(2+32, buffer.size());
    EXPECT_EQ(0xd9, buffer[0]);

    // unpack
    auto u=mpack::memory_unpacker(&buffer[0], buffer.size());
    std::string out;
    u >> out;

    EXPECT_EQ(out, str);
}

