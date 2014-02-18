
#pragma once
#include <functional>
//
// this header is generated scripts/generate_packedmethod.py
//
// don't modify by hand !
//

namespace mpack {
namespace msgpack {


//////////////////////////////////////////////////////////////////////////////
// 0 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R >
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=0){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args

            // call
            R r=f();
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R >
    packedmethod packmethod(R(*f)())
    {
        typedef R(*F)();
        return pack_internal<F, R >(f);
    }

// extract arguments and return
template<typename F, typename C, typename R >
    packedmethod pack_functor(F &f, R(C::*)()const)
    {
        return pack_internal<F, R >(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 1 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=1){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;

            // call
            R r=f(a0);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0>
    packedmethod packmethod(R(*f)(A0))
    {
        typedef R(*F)(A0);
        return pack_internal<F, R, A0>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0>
    packedmethod pack_functor(F &f, R(C::*)(A0)const)
    {
        return pack_internal<F, R, A0>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 2 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=2){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;

            // call
            R r=f(a0, a1);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1>
    packedmethod packmethod(R(*f)(A0, A1))
    {
        typedef R(*F)(A0, A1);
        return pack_internal<F, R, A0, A1>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1)const)
    {
        return pack_internal<F, R, A0, A1>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 3 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=3){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;

            // call
            R r=f(a0, a1, a2);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2>
    packedmethod packmethod(R(*f)(A0, A1, A2))
    {
        typedef R(*F)(A0, A1, A2);
        return pack_internal<F, R, A0, A1, A2>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2)const)
    {
        return pack_internal<F, R, A0, A1, A2>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 4 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2, typename A3>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=4){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;
A3 a3; u >> a3;

            // call
            R r=f(a0, a1, a2, a3);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2, typename A3>
    packedmethod packmethod(R(*f)(A0, A1, A2, A3))
    {
        typedef R(*F)(A0, A1, A2, A3);
        return pack_internal<F, R, A0, A1, A2, A3>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2, typename A3>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2, A3)const)
    {
        return pack_internal<F, R, A0, A1, A2, A3>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 5 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=5){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;
A3 a3; u >> a3;
A4 a4; u >> a4;

            // call
            R r=f(a0, a1, a2, a3, a4);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
    packedmethod packmethod(R(*f)(A0, A1, A2, A3, A4))
    {
        typedef R(*F)(A0, A1, A2, A3, A4);
        return pack_internal<F, R, A0, A1, A2, A3, A4>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2, A3, A4)const)
    {
        return pack_internal<F, R, A0, A1, A2, A3, A4>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 6 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=6){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;
A3 a3; u >> a3;
A4 a4; u >> a4;
A5 a5; u >> a5;

            // call
            R r=f(a0, a1, a2, a3, a4, a5);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    packedmethod packmethod(R(*f)(A0, A1, A2, A3, A4, A5))
    {
        typedef R(*F)(A0, A1, A2, A3, A4, A5);
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2, A3, A4, A5)const)
    {
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 7 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=7){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;
A3 a3; u >> a3;
A4 a4; u >> a4;
A5 a5; u >> a5;
A6 a6; u >> a6;

            // call
            R r=f(a0, a1, a2, a3, a4, a5, a6);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    packedmethod packmethod(R(*f)(A0, A1, A2, A3, A4, A5, A6))
    {
        typedef R(*F)(A0, A1, A2, A3, A4, A5, A6);
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5, A6>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2, A3, A4, A5, A6)const)
    {
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5, A6>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 8 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=8){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;
A3 a3; u >> a3;
A4 a4; u >> a4;
A5 a5; u >> a5;
A6 a6; u >> a6;
A7 a7; u >> a7;

            // call
            R r=f(a0, a1, a2, a3, a4, a5, a6, a7);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    packedmethod packmethod(R(*f)(A0, A1, A2, A3, A4, A5, A6, A7))
    {
        typedef R(*F)(A0, A1, A2, A3, A4, A5, A6, A7);
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5, A6, A7>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2, A3, A4, A5, A6, A7)const)
    {
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5, A6, A7>(f);
    }



//////////////////////////////////////////////////////////////////////////////
// 9 args
//////////////////////////////////////////////////////////////////////////////
// build packed method call
template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    packedmethod pack_internal(F &f)
    {
        return [f](packer &p, unpacker &u){
            // check args
            if(!u.is_array()){
                // todo
                throw std::exception(__FUNCTION__);
            }

            auto c=array();
            u >> c;
            if(c.size!=9){
                // todo
                throw std::exception(__FUNCTION__);
            }

            // unpack args
A0 a0; u >> a0;
A1 a1; u >> a1;
A2 a2; u >> a2;
A3 a3; u >> a3;
A4 a4; u >> a4;
A5 a5; u >> a5;
A6 a6; u >> a6;
A7 a7; u >> a7;
A8 a8; u >> a8;

            // call
            R r=f(a0, a1, a2, a3, a4, a5, a6, a7, a8);
            // pack result
            p << r;
        };
    }

// for function pointer
template<typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    packedmethod packmethod(R(*f)(A0, A1, A2, A3, A4, A5, A6, A7, A8))
    {
        typedef R(*F)(A0, A1, A2, A3, A4, A5, A6, A7, A8);
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5, A6, A7, A8>(f);
    }

// extract arguments and return
template<typename F, typename C, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    packedmethod pack_functor(F &f, R(C::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)const)
    {
        return pack_internal<F, R, A0, A1, A2, A3, A4, A5, A6, A7, A8>(f);
    }




// for functor
template<typename F>
    packedmethod packmethod(F &f)
    {
        return pack_functor(f, &F::operator());
    }

} // namespace
} // namespace

