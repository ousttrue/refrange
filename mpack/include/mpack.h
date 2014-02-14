#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <assert.h>


namespace mpack
{
    //////////////////////////////////////////////////////////////////////////////
    // core
    //////////////////////////////////////////////////////////////////////////////
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


    typedef std::function<size_t(const unsigned char*, size_t)> writer_t;

    class packer
    {
    public:
        writer_t m_writer;

        packer& pack_nil()
        {
            write_byte(0xc0);
            return *this;
        }

        packer& pack_bool(bool b)
        {
            write_byte(b ? 0xc3 : 0xc2);
            return *this;
        }

        packer& pack_int(int n)
        {
            if(n<0){
                if(n>-0x1f){
                    // negative fix int
                    int v = 0xe0 + (-n);
                    write_byte(static_cast<unsigned char>(v));
                    return *this;
                }
                else{
                    // not implmented
                    throw std::exception();
                }
            }
            else{
                if(n<=0x7f){
                    write_byte(static_cast<unsigned char>(n));
                    return *this;
                }
                if(n<=0xff){
                    write_byte(byte_uint8);
                    write_byte(static_cast<unsigned char>(n));
                    return *this;
                }
                else{
                    // not implmented
                    throw std::exception();
                }
            }
        }

    private:
        void write_byte(unsigned char byte)
        {
            size_t size=m_writer(&byte, 1);
            assert(size==1);
        }
    };


    typedef std::function<size_t(unsigned char*, size_t)> reader_t;


    class unpacker
    {
    public:
        reader_t m_reader;

        int unpack_int()
        {
            unsigned char head_byte=read_byte();

            switch(head_byte)
            {
                case byte_uint8:
                    {
                        return read_byte();
                    }
                    break;
            }

            if(partial_bit_equal<positive_fixint>(head_byte)){
                return static_cast<int>(head_byte);
            }
            else if(partial_bit_equal<negative_fixint>(head_byte)){
                return -static_cast<int>(head_byte & ~negative_fixint::mask);
            }
            else{
                // not implmented
                throw std::exception();
            }
        }

    private:
        unsigned char read_byte()
        {
            unsigned char byte;
            size_t size=m_reader(&byte, 1);
            assert(size==1);
            return byte;
        }
    };


    template<typename T>
        unpacker& operator>>(unpacker &unpacker, const T &t)
        {
            // not implmented
            throw std::exception();
        }

    // int
    inline unpacker& operator>>(unpacker &unpacker, int &n)
    {
        n=unpacker.unpack_int();
        return unpacker;
    }


    //////////////////////////////////////////////////////////////////////////////
    // utility
    //////////////////////////////////////////////////////////////////////////////
    class vector_packer: public packer
    {
    public:
        std::vector<unsigned char> packed_buffer;

        vector_packer()
        {
			auto buffer=&packed_buffer;
            m_writer=[buffer](const unsigned char *p, size_t size)->size_t
            {
                for(size_t i=0; i<size; ++i){
                    buffer->push_back(*p++);
                }
                return size;
            };
        }
    };


    class memory_unpacker: public unpacker
    {
        const unsigned char *m_begin;
        const unsigned char *m_p;
        size_t m_size;

    public:
        memory_unpacker(const unsigned char *begin, size_t size)
            : m_begin(begin), m_p(m_begin), m_size(size)
        {
            auto self=this;
            m_reader=[self](unsigned char *p, size_t size)->size_t
            {
                return self->read(p, size);
            };
        }

        size_t read(unsigned char *p, size_t size)
        {
            // todo: boundary check
            std::copy(m_p, m_p+size, p);
			m_p+=size;
            return size;
        }
    };
};

