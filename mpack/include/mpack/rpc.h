#pragma once


namespace mpack {
namespace msgpack {
namespace rpc {

inline void pack_request(packer &request_packer, int id, 
            const std::string &method, 
            packer &args_packer)
{
    request_packer 
        << array(4)
        << 0
        << id
        << method
        << array(args_packer)
        ;
}

inline void pack_response(packer &response_packer, int id, 
            packer &result_packer)
{
    response_packer
        << array(4)
        << 1
        << id
        << nil // no error
        ;
    if(result_packer.size()==0){
        response_packer << nil;
    }
    else{
        response_packer << result_packer;
    }
}

} // rpc
} // msgpack
} // mpack
