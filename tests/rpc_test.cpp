#include <mpack.h>
#include <gtest/gtest.h>
#include <map>


static int add(int a, int b)
{
    return a+b;
}


class dispatcher
{
    std::map<std::string, mpack::msgpack::packedmethod> m_map;
public:
    void register_method(const std::string &name, const mpack::msgpack::packedmethod &m)
    {
        m_map[name]=m;
    }

    void dispatch(mpack::msgpack::packer &response, 
            const unsigned char *request, size_t len)
    {
        // unpack request
        auto u=mpack::msgpack::create_memory_unpacker(request, len);
        if(!u.is_array()){
            throw std::exception(__FUNCTION__);
        }

        auto c=mpack::msgpack::array();
        u >> c;
        if(c.size!=4){
            throw std::exception(__FUNCTION__);
        }

        int t;
        u >> t;
        if(t!=0){
            throw std::exception(__FUNCTION__);
        }

        int id;
        u >> id;

        std::string name;
        u >> name;

        auto it=m_map.find(name);
        if(it==m_map.end()){
            // not found
            throw std::exception(__FUNCTION__);
        }

        // call
        auto p=mpack::msgpack::create_vector_packer();
        it->second(p, u);

        // pack result
        mpack::msgpack::rpc::pack_response(response, id, p);
    }
};


TEST(RPC, fp_2to1) 
{
    // regsiter functions
    auto method=mpack::msgpack::packmethod(add);
    dispatcher d;
    d.register_method("add", method);

    // pack args 
    auto args_packer=mpack::msgpack::create_vector_packer();
    args_packer << 1 << 2;

    // request
    int request_id=99;
    std::vector<unsigned char> request_message;
    auto request_packer=mpack::msgpack::create_external_vector_packer(request_message);
    // [type(int)=0, msgid(int), method(string), params(array)]
    mpack::msgpack::rpc::pack_request(request_packer, request_id, "add", args_packer);

    //////////////////////////////////////////////////////////////////////
    // call
    std::vector<unsigned char> response_message;
    auto response_packer=mpack::msgpack::create_external_vector_packer(response_message);
    d.dispatch(response_packer, request_packer.pointer(), request_packer.size());
    //////////////////////////////////////////////////////////////////////

    // unpack result
    auto response_unpacker=mpack::msgpack::create_memory_unpacker(
            response_packer.pointer(), response_packer.size());

    // [type(int)=1, msgid(int), error(nil or not), result(any)]
    ASSERT_TRUE(response_unpacker.is_array());

    auto array=mpack::msgpack::array();
    response_unpacker >> array;
    ASSERT_EQ(4, array.size);

    int type;
    response_unpacker >> type;
    ASSERT_EQ(1, type);

    int response_id;
    response_unpacker >> response_id;
    ASSERT_EQ(request_id, response_id);

    ASSERT_TRUE(response_unpacker.is_nil());
    response_unpacker.unpack_nil();

    int n;
    response_unpacker >> n;
    EXPECT_EQ(3, n);
}

