#include <mpack.h>
#include <refrange/range.h>
#include <gtest/gtest.h>

/// nil:
/// +--------+
/// |  0xc0  |
/// +--------+
TEST(MsgpackTest, nil) 
{
    unsigned char buf[256]={0};

    refrange::mutable_range r(buf, buf+256);

    // packing
    auto p=mpack::msgpack::create_packer(r);
    p.pack_nil();

    // check
    ASSERT_EQ(1, p.size());
    EXPECT_EQ(0xc0, buf[0]);
}

/// false:
/// +--------+
/// |  0xc2  |
/// +--------+
TEST(MsgpackTest, false)
{
    // packing
    auto p=mpack::msgpack::create_vector_packer();
    p.pack_bool(false);

    // check
    ASSERT_EQ(1, p.size());
    EXPECT_EQ(0xc2, *p.pointer());
}

/// true:
/// +--------+
/// |  0xc3  |
/// +--------+
TEST(MsgpackTest, true)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p.pack_bool(true);

    // check
    ASSERT_EQ(1, p.size());
    EXPECT_EQ(0xc3, *p.pointer());
}

/// positive fixnum stores 7-bit positive integer
/// +--------+
/// |0XXXXXXX|
/// +--------+
TEST(MsgpackTest, small_int)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p << 1;

    // check
    ASSERT_EQ(1, p.size());
	ASSERT_TRUE(mpack::msgpack::positive_fixint_tag::is_match(*p.pointer()));

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    int n=0;
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
	auto p=mpack::msgpack::create_vector_packer();
    p << -1;

    // check
    ASSERT_EQ(1, p.size());
	ASSERT_TRUE(mpack::msgpack::negative_fixint_tag::is_match(*p.pointer()));

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << 128;

    // check
    ASSERT_EQ(2, p.size());
    EXPECT_EQ(0xcc, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    unsigned int n=0;
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
	auto p=mpack::msgpack::create_vector_packer();
    p << 256;

    // check
    ASSERT_EQ(3, p.size());
    EXPECT_EQ(0xcd, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    unsigned int n=0;
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
		auto p=mpack::msgpack::create_vector_packer();
        p << value;

        // check
        ASSERT_EQ(5, p.size());
        EXPECT_EQ(0xce, *p.pointer());

        // unpack
		auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
        unsigned int n=0;
        u >> n;
        EXPECT_EQ(value, n);
    }

    {
        unsigned int value=4294967295;

        // packing
		auto p=mpack::msgpack::create_vector_packer();
        p << value;

        // check
        ASSERT_EQ(5, p.size());
        EXPECT_EQ(0xce, *p.pointer());

        // unpack
		auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
        unsigned int n=0;
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(9, p.size());
    EXPECT_EQ(0xcf, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(2, p.size());
    EXPECT_EQ(0xd0, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(3, p.size());
    EXPECT_EQ(0xd1, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(5, p.size());
    EXPECT_EQ(0xd2, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(9, p.size());
    EXPECT_EQ(0xd3, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(5, p.size());
    EXPECT_EQ(0xca, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << value;

    // check
    ASSERT_EQ(9, p.size());
    EXPECT_EQ(0xcb, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << str;

    // check
    ASSERT_EQ(1+3, p.size());
	ASSERT_TRUE(mpack::msgpack::fixstr_tag::is_match(*p.pointer()));

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
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
	auto p=mpack::msgpack::create_vector_packer();
    p << str;

    // check
    ASSERT_EQ(2+32, p.size());
    EXPECT_EQ(0xd9, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    std::string out;
    u >> out;

    EXPECT_EQ(out, str);
}

/// str 16 stores a byte array whose length is upto (2^16)-1 bytes:
/// +--------+--------+--------+========+
/// |  0xda  |ZZZZZZZZ|ZZZZZZZZ|  data  |
/// +--------+--------+--------+========+
TEST(MsgpackTest, str16)
{
    auto str=
        "0123456789" "0123456789" "0123456789" "0123456789" "0123456789" "0123456789"
        "0123456789" "0123456789" "0123456789" "0123456789" "0123456789" "0123456789"
        "0123456789" "0123456789" "0123456789" "0123456789" "0123456789" "0123456789"
        "0123456789" "0123456789" "0123456789" "0123456789" "0123456789" "0123456789"
        "0123456789" "012345"
        ;

    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p << str;

    // check
    ASSERT_EQ(3+256, p.size());
    EXPECT_EQ(0xda, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    std::string out;
    u >> out;

    EXPECT_EQ(out, str);
}

/// str 32 stores a byte array whose length is upto (2^32)-1 bytes:
/// +--------+--------+--------+--------+--------+========+
/// |  0xdb  |AAAAAAAA|AAAAAAAA|AAAAAAAA|AAAAAAAA|  data  |
/// +--------+--------+--------+--------+--------+========+
TEST(MsgpackTest, str32)
{
    std::string str;
    for(int i=0; i<(0xFFFF+1); ++i)
    {
        str.push_back('0'+(i%10));
    }

    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p << str;

    // check
    ASSERT_EQ(5+0xFFFF+1, p.size());
    EXPECT_EQ(0xdb, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    std::string out;
    u >> out;

    EXPECT_EQ(out, str);
}

/// bin 8 stores a byte array whose length is upto (2^8)-1 bytes:
/// +--------+--------+========+
/// |  0xc4  |XXXXXXXX|  data  |
/// +--------+--------+========+
TEST(MsgpackTest, bin8)
{
    std::vector<unsigned char> buf;
    buf.push_back(0);

    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p << buf;

    // check
    ASSERT_EQ(2+1, p.size());
    EXPECT_EQ(0xc4, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    std::vector<unsigned char> out;
    u >> out;

    EXPECT_EQ(buf, out);
}

/// bin 16 stores a byte array whose length is upto (2^16)-1 bytes:
/// +--------+--------+--------+========+
/// |  0xc5  |YYYYYYYY|YYYYYYYY|  data  |
/// +--------+--------+--------+========+
TEST(MsgpackTest, bin16)
{
    std::vector<unsigned char> buf;
    for(int i=0; i<0xFF+1; ++i){
        buf.push_back(i % 0xFF);
    }

    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p << buf;

    // check
    ASSERT_EQ(3+0xFF+1, p.size());
    EXPECT_EQ(0xc5, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    std::vector<unsigned char> out;
    u >> out;

    EXPECT_EQ(buf, out);
}

/// bin 32 stores a byte array whose length is upto (2^32)-1 bytes:
/// +--------+--------+--------+--------+--------+========+
/// |  0xc6  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|  data  |
/// +--------+--------+--------+--------+--------+========+
TEST(MsgpackTest, bin32)
{
    std::vector<unsigned char> buf;
    for(int i=0; i<0xFFFF+1; ++i){
        buf.push_back(i % 0xFF);
    }

    // packing
	auto p=mpack::msgpack::create_vector_packer();
    p << buf;

    // check
    ASSERT_EQ(5+0xFFFF+1, p.size());
    EXPECT_EQ(0xc6, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    std::vector<unsigned char> out;
    u >> out;

    EXPECT_EQ(buf, out);
}

/// fixarray stores an array whose length is upto 15 elements:
/// +--------+~~~~~~~~~~~~~~~~~+
/// |1001XXXX|    N objects    |
/// +--------+~~~~~~~~~~~~~~~~~+
TEST(MsgpackTest, fixarray)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
	p << mpack::msgpack::array(3)
        << 1 << "str" << true
		;


    // check
	ASSERT_TRUE(mpack::msgpack::fixarray_tag::is_match(*p.pointer()));

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    EXPECT_TRUE(u.is_array());

	auto c = mpack::msgpack::array();
    int n;
    std::string str;
    bool b;
    u >> c >> n >> str >> b;

    EXPECT_EQ(3, c.size);
    EXPECT_EQ(1, n);
    EXPECT_EQ("str", str);
    EXPECT_EQ(true, b);
}

/// array 16 stores an array whose length is upto (2^16)-1 elements:
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xdc  |YYYYYYYY|YYYYYYYY|    N objects    |
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
TEST(MsgpackTest, array16)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
	p << mpack::msgpack::array(16)
        << 1 << "str1" << true << 1.5f
        << 2 << "str2" << false << 1.6f
        << 3 << "str3" << true << 1.7
        << 4 << "str4" << false << 1.8
		;

    // check
    EXPECT_EQ(0xdc, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    EXPECT_TRUE(u.is_array());

    // array
	auto c = mpack::msgpack::array();
    u >> c ;
    EXPECT_EQ(16, c.size);

    {
        int n;
        std::string str;
        bool b;
        double f;
        u >> n >> str >> b >> f
            ;
        EXPECT_EQ(1, n);
        EXPECT_EQ("str1", str);
        EXPECT_EQ(true, b);
        EXPECT_EQ(1.5f, f);
    }
}

/// array 32 stores an array whose length is upto (2^32)-1 elements:
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xdd  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|    N objects    |
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
TEST(MsgpackTest, array32)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
	auto pc = mpack::msgpack::array(0xFFFF + 1);
    p << pc;
    for(size_t i=0; i<pc.size; ++i){
        p << i;
    }

    // check
    EXPECT_EQ(0xdd, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    EXPECT_TRUE(u.is_array());

    // array
	auto uc = mpack::msgpack::array();
    u >> uc ;
    EXPECT_EQ(pc.size, uc.size);

    for(size_t i=0; i<uc.size; ++i){
        int n;
        u >> n;
        EXPECT_EQ(i, n);
    }
}

/// fixmap stores a map whose length is upto 15 elements
/// +--------+~~~~~~~~~~~~~~~~~+
/// |1000XXXX|   N*2 objects   |
/// +--------+~~~~~~~~~~~~~~~~~+
TEST(MsgpackTest, fixmap)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
	p << mpack::msgpack::map(3)
        << "key1" << 0
        << "key2" << 1
        << "key3" << 2
		;

    // check
	ASSERT_TRUE(mpack::msgpack::fixmap_tag::is_match(*p.pointer()));

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    EXPECT_TRUE(u.is_map());

	auto c = mpack::msgpack::map();
    u >> c;
    EXPECT_EQ(3, c.size);

    for(size_t i=0; i<c.size; ++i){
        std::string key;
        int value;
        u >> key >> value;
        std::stringstream ss;
        ss << "key" << (i+1);
        EXPECT_EQ(ss.str(), key);
        EXPECT_EQ(i, value);
    }
}

/// map 16 stores a map whose length is upto (2^16)-1 elements
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xde  |YYYYYYYY|YYYYYYYY|   N*2 objects   |
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
TEST(MsgpackTest, map16)
{
    // packing
	auto p=mpack::msgpack::create_vector_packer();
	p << mpack::msgpack::map(17)
        << "key1" << 0 << "key2" << 1 << "key3" << 2 << "key4" << 3
        << "key5" << 4 << "key6" << 5 << "key7" << 6 << "key8" << 7
        << "key9" << 8 << "key10" << 9 << "key11" << 10 << "key12" << 11
        << "key13" << 12 << "key14" << 13 << "key15" << 14 << "key16" << 15
        << "key17" << 16
		;

    // check
    EXPECT_EQ(0xde, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    EXPECT_TRUE(u.is_map());

    // map
	auto c = mpack::msgpack::map();
    u >> c ;
    EXPECT_EQ(17, c.size);

    for(size_t i=0; i<c.size; ++i){
        std::string key;
        int value;
        u >> key >> value;
        std::stringstream ss;
        ss << "key" << (i+1);
        EXPECT_EQ(ss.str(), key);
        EXPECT_EQ(i, value);
    }
}

/*
/// map 32 stores a map whose length is upto (2^32)-1 elements
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xdf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|   N*2 objects   |
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
TEST(MsgpackTest, map32)
{
    // packing
	mpack::msgpack::external_vector_packer p;
	auto pc = mpack::msgpack::map_context(0xFFFF + 1);
    p << pc; 
    for(int i=0; i<pc.size; ++i){
        std::stringstream ss;
        ss << "key" << (i+1);
        p << ss.str() << i;
    }

    // check
    EXPECT_EQ(0xdf, *p.pointer());

    // unpack
	auto u = mpack::msgpack::create_unpacker(p.pointer(), p.size());
    EXPECT_TRUE(u.is_map());

    // map
	auto uc = mpack::msgpack::map_context();
    u >> uc ;
    EXPECT_EQ(pc.size, uc.size);

    for(int i=0; i<uc.size; ++i){
        std::string key;
        int value;
        u >> key >> value;
        std::stringstream ss;
        ss << "key" << (i+1);
        EXPECT_EQ(ss.str(), key);
        EXPECT_EQ(i, value);
    }
}
*/
