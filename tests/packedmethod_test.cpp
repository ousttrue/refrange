#include <mpack.h>
#include <gtest/gtest.h>

static int add(int a, int b)
{
    return a+b;
}

TEST(PackedMethod, method2to1) 
{
    // wrap static function pointer
    auto method=mpack::msgpack::packmethod(std::function<int(int,int)>(add));
	std::vector<unsigned char> args_buffer;
	std::vector<unsigned char> result_buffer;

    // pack args 
    {
        mpack::msgpack::vector_packer args_packer(args_buffer);
        args_packer << 1 << 2;
    }

    // call method(packing result to result_packer)
    {
        mpack::msgpack::vector_packer result_packer(result_buffer);
		mpack::msgpack::memory_unpacker args_unpacker(&args_buffer[0], args_buffer.size());
        method(result_packer, args_unpacker);
    }

    // unpack result
	auto u = mpack::msgpack::memory_unpacker(&result_buffer[0], result_buffer.size());
    int n;
    u >> n;
    EXPECT_EQ(3, n);
}

