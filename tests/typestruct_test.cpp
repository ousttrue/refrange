#include <mpack.h>
#include <gtest/gtest.h>


TEST(TypeStructTest, sample) 
{
	std::vector<unsigned char> buffer;

    // packing
    mpack::msgpack::vector_packer p(buffer);
    p << 1;

    // unpack
	auto u = mpack::msgpack::memory_unpacker(&buffer[0], buffer.size());

    std::stringstream ss;
    mpack::msgpack::typestruct(u, ss);

    EXPECT_STREQ("int", ss.str().c_str());
}

