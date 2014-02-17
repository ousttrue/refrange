#pragma once
#include <functional>


namespace mpack {
namespace msgpack {

	typedef std::function<void(packer&, unpacker&)> packedmethod;

    template<typename R, typename A1, typename A2>
        packedmethod packmethod(std::function<R(A1,A2)> &f)
        {
            return [f](packer &p, unpacker &u){
                // unpack args
                A1 a1; A2 a2;
                u >> a1 >> a2;
                // call
                R r=f(a1, a2);
                // pack result
                p << r;
            };
        }

} // namespace
} // namespace
