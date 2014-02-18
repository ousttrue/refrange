#pragma once
#include <functional>


namespace mpack {
namespace msgpack {

	typedef std::function<void(packer&, unpacker&)> packedmethod;

    // build packed method call
    template<typename F, typename R, typename A1, typename A2>
        packedmethod pack_internal(F &f)
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

    // for function pointer
    template<typename R, typename A1, typename A2>
        packedmethod packmethod(R(*f)(A1,A2))
        {
			typedef R(*F)(A1, A2);
            return pack_internal<F, R, A1, A2>(f);
        }

    // extract arguments and return
    template<typename F, typename C, typename R, typename A1, typename A2>
        packedmethod pack_functor(F &f, R(C::*)(A1,A2)const)
        {
            return pack_internal<F, R, A1, A2>(f);
        }

    // for functor
    template<typename F>
        packedmethod packmethod(F &f)
        {
            return pack_functor(f, &F::operator());
        }

} // namespace
} // namespace
