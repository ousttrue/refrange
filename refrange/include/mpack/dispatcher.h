#pragma once
#include <map>

namespace mpack {
namespace msgpack {

	using namespace refrange::msgpack;

namespace rpc {

class dispatcher
{
    std::map<std::string, packedmethod> m_map;
public:
    void register_method(const std::string &name, const packedmethod &m)
    {
        m_map[name]=m;
    }

    void push(const unsigned char *p, size_t len)
    {
    }

    /*
    void dispatch(packer &response, 
            const unsigned char *request, size_t len)
    {
        // unpack request
        auto u=create_memory_unpacker(request, len);
        dispatch(response, u);
    }
    */

    void dispatch(packer &response, unpacker &u)
    {
        if(!u.is_array()){
            throw std::exception(__FUNCTION__);
        }

        auto c=array();
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
        auto p=create_vector_packer();
        it->second(p, u);

        // pack result
        rpc::pack_response(response, id, p);
    }
};

} // namespace
} // namespace
} // namespace
