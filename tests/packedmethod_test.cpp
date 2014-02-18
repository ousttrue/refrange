#include <mpack.h>
#include <gtest/gtest.h>

static int add(int a, int b)
{
    return a+b;
}

TEST(PackedMethod, fp_2to1) 
{
    // wrap static function pointer
    auto method=mpack::msgpack::packmethod(add);

    // pack args 
    auto args_packer=mpack::msgpack::create_vector_packer();
    args_packer << mpack::msgpack::array(2) << 1 << 2;

    // call method(packing result to result_packer)
    auto result_packer=mpack::msgpack::create_vector_packer();
	auto args_unpacker = mpack::msgpack::create_memory_unpacker (args_packer.pointer(), args_packer.size());
    method(result_packer, args_unpacker);

    // unpack result
	auto u = mpack::msgpack::create_memory_unpacker(result_packer.pointer(), result_packer.size());
    int n;
    u >> n;
    EXPECT_EQ(3, n);
}

TEST(PackedMethod, lambda_2to1) 
{
    auto add_lambda=[](int a, int b){ return a+b; };
    auto method=mpack::msgpack::packmethod(add_lambda);

    // pack args 
    auto args_packer=mpack::msgpack::create_vector_packer();
    args_packer << mpack::msgpack::array(2) << 1 << 2;

    // call method(packing result to result_packer)
    auto result_packer=mpack::msgpack::create_vector_packer();
	auto args_unpacker = mpack::msgpack::create_memory_unpacker (args_packer.pointer(), args_packer.size());
    method(result_packer, args_unpacker);

    // unpack result
	auto u = mpack::msgpack::create_memory_unpacker(result_packer.pointer(), result_packer.size());
    int n;
    u >> n;
    EXPECT_EQ(3, n);
}

