#pragma once
#include <functional>


namespace mpack {
namespace msgpack {

	typedef std::function<void(packer&, unpacker&)> packedmethod;

    // for function pointer
    template<typename R, typename A1, typename A2>
        packedmethod packmethod(R(*f)(A1,A2))
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

    // for functor 2
    template<typename F, typename C, typename R, typename A1, typename A2>
        packedmethod _packmethod(F &f, R(C::*)(A1,A2)const)
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

    // for functor 1
    template<typename F>
        packedmethod packmethod(F &f)
        {
            return _packmethod(f, &F::operator());
        }

} // namespace
} // namespace
