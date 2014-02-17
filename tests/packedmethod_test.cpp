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

    // pack args 
    auto args_packer=mpack::msgpack::vector_packer();
    args_packer << 1 << 2;

    // call method(packing result to result_packer)
    auto result_packer=mpack::msgpack::vector_packer();
    mpack::msgpack::memory_unpacker args_unpacker(args_packer.pointer(), args_packer.size());
    method(result_packer, args_unpacker);

    // unpack result
	auto u = mpack::msgpack::memory_unpacker(result_packer.pointer(), result_packer.size());
    int n;
    u >> n;
    EXPECT_EQ(3, n);
}

