#pragma once
#include <vector>


namespace mpack
{
    struct PositiveFixint
    {
        enum bits_t { bits=0x00 };
        enum mask_t { mask=0x80 };
    };
    struct Fixmap
    {
        enum bits_t { bits=0x80 };
        enum mask_t { mask=0xF0 };
    };
    struct Fixarray
    {
        enum bits_t { bits=0x90 };
        enum mask_t { mask=0xF0 };
    };
    struct Fixstr
    {
        enum bits_t { bits=0xa0 };
        enum mask_t { mask=0xE0};
    };
    struct NegativeFixint
    {
        enum bits_t { bits=0xe0 };
        enum mask_t { mask=0xE0 };
    };

    template<class T>
        bool partial_bit_equal(unsigned char byte)
        {
            return T::bits==(T::mask & byte);
        }

    enum type
    {
        Nil=0xc0,
        False=0xc2,
        True=0xc3,
        Bin8=0xc4,
        Bin16=0xc5,
        Bin32=0xc6,
        Ext8=0xc7,
        Ext16=0xc8,
        Ext32=0xc9,
        Float32=0xca,
        Float64=0xcb,
        Uint8=0xcc,
        Uint16=0xcd,
        Uint32=0xce,
        Uint64=0xcf,
        Int8=0xd0,
        Int16=0xd1,
        Int32=0xd2,
        Int64=0xd3,
        Fixext1=0xd4,
        Fixext2=0xd5,
        Fixext4=0xd6,
        Fixext8=0xd7,
        Fixext16=0xd8,
        Str8=0xd9,
        Str16=0xda,
        Str32=0xdb,
        Array16=0xdc,
        Array32=0xdd,
        Map16=0xde,
        Map32=0xdf,
    };

    class packer
    {
        public:
            std::vector<unsigned char> packed_buffer;

            packer& pack_nil()
            {
                packed_buffer.push_back(0xc0);
                return *this;
            }

            packer& pack_bool(bool b)
            {
                packed_buffer.push_back(b ? 0xc3 : 0xc2);
                return *this;
            }

            packer& pack_int(int n)
            {
                if(n<0){
                    if(n>-0x1f){
                        // negative fix int
						int v = 0xe0 + (-n);
                        packed_buffer.push_back(static_cast<unsigned char>(v));
                        return *this;
                    }
                    else{
                        // not implmented
                        throw std::exception();
                    }
                }
                else{
                    if(n<=0x7f){
                        packed_buffer.push_back(n);
                        return *this;
                    }
                    else{
                        // not implmented
                        throw std::exception();
                    }
                }
            }
    };

    template<typename T>
        packer& operator<<(packer &packer, const T &t)
        {
            // not implmented
            throw std::exception();
        }

    // int
    inline packer& operator<<(packer &packer, int n)
    {
        packer.packed_buffer.push_back(n);
        return packer;
    };

    class reference_unpacker
    {
        const unsigned char *m_begin;
        const unsigned char *m_p;
        size_t m_size;

        public:
        reference_unpacker(const unsigned char*p, size_t size)
            : m_begin(p), m_p(m_begin), m_size(size)
        {
        };

        int get_int()
        {
            if(m_p>=m_begin+m_size){
                // over
                throw std::exception();
            }

            if(partial_bit_equal<PositiveFixint>(*m_p)){
                unsigned char n=*m_p++;
                return static_cast<int>(n);
            }
            else if(partial_bit_equal<NegativeFixint>(*m_p)){
                unsigned char n=*m_p++;
                return -static_cast<int>(n & ~NegativeFixint::mask);
            }
            else{
                // not implmented
                throw std::exception();
            }
        }
    };

    template<typename T>
        reference_unpacker& operator>>(reference_unpacker &unpacker, const T &t)
        {
            // not implmented
            throw std::exception();
        }

    // int
    inline reference_unpacker& operator>>(reference_unpacker &unpacker, int &n)
    {
        n=unpacker.get_int();
        return unpacker;
    }

};

