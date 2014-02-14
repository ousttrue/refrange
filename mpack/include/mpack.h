#pragma once
#include <vector>


namespace mpack
{
    struct positive_fixint
    {
        enum bits_t { bits=0x00 };
        enum mask_t { mask=0x80 };
    };
    struct fixmap
    {
        enum bits_t { bits=0x80 };
        enum mask_t { mask=0xF0 };
    };
    struct fixarray
    {
        enum bits_t { bits=0x90 };
        enum mask_t { mask=0xF0 };
    };
    struct fixstr
    {
        enum bits_t { bits=0xa0 };
        enum mask_t { mask=0xE0};
    };
    struct negative_fixint
    {
        enum bits_t { bits=0xe0 };
        enum mask_t { mask=0xE0 };
    };

    template<class T>
        bool partial_bit_equal(unsigned char byte)
        {
            return T::bits==(T::mask & byte);
        }

    enum byte_type
    {
        byte_nil=0xc0,
        byte_false=0xc2,
        byte_true=0xc3,
        byte_bin8=0xc4,
        byte_bin16=0xc5,
        byte_bin32=0xc6,
        byte_ext8=0xc7,
        byte_ext16=0xc8,
        byte_ext32=0xc9,
        byte_float32=0xca,
        byte_float64=0xcb,
        byte_uint8=0xcc,
        byte_uint16=0xcd,
        byte_uint32=0xce,
        byte_uint64=0xcf,
        byte_int8=0xd0,
        byte_int16=0xd1,
        byte_int32=0xd2,
        byte_int64=0xd3,
        byte_fixext1=0xd4,
        byte_fixext2=0xd5,
        byte_fixext4=0xd6,
        byte_fixext8=0xd7,
        byte_fixext16=0xd8,
        byte_str8=0xd9,
        byte_str16=0xda,
        byte_str32=0xdb,
        byte_array16=0xdc,
        byte_array32=0xdd,
        byte_map16=0xde,
        byte_map32=0xdf,
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
                    if(n<=0xff){
						packed_buffer.push_back(byte_uint8);
                        packed_buffer.push_back(static_cast<unsigned char>(n));
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

            switch(*m_p)
            {
                case byte_uint8:
                    {
                        ++m_p;
                        unsigned char n=*m_p++;
                        return n;
                    }
                    break;
            }

            if(partial_bit_equal<positive_fixint>(*m_p)){
                unsigned char n=*m_p++;
                return static_cast<int>(n);
            }
            else if(partial_bit_equal<negative_fixint>(*m_p)){
                unsigned char n=*m_p++;
                return -static_cast<int>(n & ~negative_fixint::mask);
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

