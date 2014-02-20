#include <refrange/msgpack/typestruct.h>
#include <refrange/msgpack/utility.h>
#include <gtest/gtest.h>


TEST(TypeStructTest, sample) 
{
    // packing
    auto p=refrange::msgpack::create_vector_packer();
    p << 1;

    // unpack
	auto u = refrange::msgpack::create_unpacker(p.pointer(), p.size());

    std::stringstream ss;
    refrange::msgpack::typestruct(u, ss);

    EXPECT_STREQ("int", ss.str().c_str());
}

TEST(TypeStructTest, array) 
{
    // packing
    auto p=refrange::msgpack::create_vector_packer();
    p << refrange::msgpack::array(2) << 1 << 2;

    // unpack
	auto u = refrange::msgpack::create_unpacker(p.pointer(), p.size());

    std::stringstream ss;
    refrange::msgpack::typestruct(u, ss);

    EXPECT_STREQ("[int,int]", ss.str().c_str());
}

TEST(TypeStructTest, vec3) 
{
    // packing
    auto p=refrange::msgpack::create_vector_packer();
    p << refrange::msgpack::array(3) << 1.0f << 1.0f << 1.0f;

    // unpack
	auto u = refrange::msgpack::create_unpacker(p.pointer(), p.size());

    std::stringstream ss;
    refrange::msgpack::typestruct(u, ss);

    EXPECT_STREQ("[float,float,float]", ss.str().c_str());
}

TEST(TypeStructTest, map) 
{
    // packing
    auto p=refrange::msgpack::create_vector_packer();
    p << refrange::msgpack::map(1) << "key" << 1;

    // unpack
	auto u = refrange::msgpack::create_unpacker(p.pointer(), p.size());

    std::stringstream ss;
    refrange::msgpack::typestruct(u, ss);

    EXPECT_STREQ("{string:int}", ss.str().c_str());
}

TEST(TypeStructTest, nest) 
{
    // packing
    auto p=refrange::msgpack::create_vector_packer();
    p << refrange::msgpack::map(1) 
        << "key" 
        << refrange::msgpack::array(1) 
        << 1;

    // unpack
	auto u = refrange::msgpack::create_unpacker(p.pointer(), p.size());

    std::stringstream ss;
    refrange::msgpack::typestruct(u, ss);

    EXPECT_STREQ("{string:[int]}", ss.str().c_str());
}

