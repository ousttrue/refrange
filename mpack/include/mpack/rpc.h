#pragma once


namespace mpack {
namespace msgpack {
namespace rpc {


struct rpc_error: public std::invalid_argument
{
    rpc_error(const std::string &message)
        : std::invalid_argument(message)
    {}
};


inline void pack_request(packer &request_packer, int id, 
            const std::string &method, 
            packer &args_packer)
{
    request_packer 
        << array(4)
        << 0 //1
        << id //2
        << method //3
        << array(args_packer) //4
        ;
}

inline void pack_response(packer &response_packer, int id, 
            packer &result_packer)
{
    response_packer
        << array(4)
        << 1 //1
        << id //2
        ;
    response_packer.pack_nil();
    if(result_packer.size()==0){
        result_packer.pack_nil();
    }
    else{
        response_packer << result_packer;
    }
}

} // rpc
} // msgpack
} // mpack
