#include <mpack.h>
#include <gtest/gtest.h>


TEST(TypeStructTest, sample) 
{
	std::vector<unsigned char> buffer;

    // packing
    mpack::msgpack::external_vector_packer p(buffer);
    p << 1;

    // unpack
	auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

    std::stringstream ss;
    mpack::msgpack::typestruct(u, ss);

    EXPECT_STREQ("int", ss.str().c_str());
}

TEST(TypeStructTest, array) 
{
	std::vector<unsigned char> buffer;

    // packing
    mpack::msgpack::external_vector_packer p(buffer);
    p << mpack::msgpack::array(2) << 1 << 2;

    // unpack
	auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

    std::stringstream ss;
    mpack::msgpack::typestruct(u, ss);

    EXPECT_STREQ("[int,int]", ss.str().c_str());
}

TEST(TypeStructTest, vec3) 
{
	std::vector<unsigned char> buffer;

    // packing
    mpack::msgpack::external_vector_packer p(buffer);
    p << mpack::msgpack::array(3) << 1.0f << 1.0f << 1.0f;

    // unpack
	auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

    std::stringstream ss;
    mpack::msgpack::typestruct(u, ss);

    EXPECT_STREQ("[float,float,float]", ss.str().c_str());
}

TEST(TypeStructTest, map) 
{
	std::vector<unsigned char> buffer;

    // packing
    mpack::msgpack::external_vector_packer p(buffer);
    p << mpack::msgpack::map(1) << "key" << 1;

    // unpack
	auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

    std::stringstream ss;
    mpack::msgpack::typestruct(u, ss);

    EXPECT_STREQ("{string:int}", ss.str().c_str());
}

TEST(TypeStructTest, nest) 
{
	std::vector<unsigned char> buffer;

    // packing
    mpack::msgpack::external_vector_packer p(buffer);
    p << mpack::msgpack::map(1) 
        << "key" 
        << mpack::msgpack::array(1) 
        << 1;

    // unpack
	auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

    std::stringstream ss;
    mpack::msgpack::typestruct(u, ss);

    EXPECT_STREQ("{string:[int]}", ss.str().c_str());
}

