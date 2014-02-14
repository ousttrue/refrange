#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
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
        enum mask_t { mask=0xf0 };
    };
    struct fixstr
    {
        enum bits_t { bits=0xa0 };
        enum mask_t { mask=0xe0};
    };
    struct negative_fixint
    {
        enum bits_t { bits=0xe0 };
        enum mask_t { mask=0xe0 };
    };

    template<class T>
        bool partial_bit_equal(unsigned char byte)
        {
            return T::bits==(T::mask & byte);
        }

    template<class T>
        char extract_head_byte(unsigned char byte)
        {
            return static_cast<char>(byte & ~T::mask);
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
            write_head_byte(byte_nil);
            return *this;
        }

        packer& pack_bool(bool b)
        {
            write_head_byte(b ? byte_true : byte_false);
            return *this;
        }

        template<typename T>
        packer& pack_int(T n)
        {
            if(n<0){
                if(n>-0x1f){
                    // negative fix int
                    auto v = negative_fixint::bits | -static_cast<char>(n);
                    write_value(static_cast<char>(v));
                    return *this;
                }
                if(n>-0xff){
                    // int8
                    write_head_byte(byte_int8);
                    write_value(static_cast<char>(n));
                    return *this;
                }
                if(n>-0xffff){
                    // int16
                    write_head_byte(byte_int16);
                    write_value(static_cast<short>(n));
                    return *this;
                }
                if(n>-0xffffffff){
                    // int32
                    write_head_byte(byte_int32);
                    write_value(static_cast<int>(n));
                    return *this;
                }
                else{
                    // int64
                    write_head_byte(byte_int64);
                    write_value(static_cast<long long>(n));
                    return *this;
                }
            }
            else{
                if(n<=0x7f){
                    // 7bit byte
                    write_value(static_cast<unsigned char>(n));
                    return *this;
                }
                else if(n<=0xff){
                    // uint8
                    write_head_byte(byte_uint8);
                    write_value(static_cast<unsigned char>(n));
                    return *this;
                }
                if(n<=0xffff){
                    // uint16
                    write_head_byte(byte_uint16);
                    write_value(static_cast<unsigned short>(n));
                    return *this;
                }
                else if(n<=0xffffffff){
                    // uint32
                    write_head_byte(byte_uint32);
                    write_value(static_cast<unsigned int>(n));
                    return *this;
                }
                else{
                    // uint64
                    write_head_byte(byte_uint64);
                    write_value(static_cast<unsigned long long>(n));
                    return *this;
                }
            }
        }

        packer& pack_float(float n)
        {
            write_head_byte(byte_float32);
            write_value(n);
            return *this;
        }

        packer& pack_double(double n)
        {
            write_head_byte(byte_float64);
            write_value(n);
            return *this;
        }

        packer& pack_str(const char *p)
        {
            return pack_str(p, strlen(p));
        }

        packer& pack_str(const char *p, size_t len)
        {
            if(len<32){
                // fixstr
                auto v = fixstr::bits | len;
                write_value(static_cast<char>(v));
                size_t size=m_writer((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xff){
                // str8
                write_head_byte(byte_str8);
                write_value(static_cast<unsigned char>(len));
                size_t size=m_writer((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffff){
                // str16
                write_head_byte(byte_str16);
                write_value(static_cast<unsigned short>(len));
                size_t size=m_writer((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffffffff){
                // str32
                write_head_byte(byte_str32);
                write_value(static_cast<unsigned int>(len));
                size_t size=m_writer((unsigned char*)p, len);
                assert(size==len);
            }
            else{
                throw std::out_of_range(__FUNCTION__);
            }
            return *this;
        }

    private:
        void write_head_byte(byte_type head_byte)
        {
            write_value(static_cast<unsigned char>(head_byte));
        }

        template<typename T>
        void write_value(T n)
        {
            size_t size=m_writer((unsigned char*)&n, sizeof(T));
            assert(size==sizeof(T));
        }
    };

    inline packer& operator<<(packer &packer, char t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, short t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, int t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, long long t) { return packer.pack_int(t); }

    inline packer& operator<<(packer &packer, unsigned char t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, unsigned short t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, unsigned int t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, unsigned long long t) { return packer.pack_int(t); }

    inline packer& operator<<(packer &packer, const float t) { return packer.pack_float(t); }
    inline packer& operator<<(packer &packer, const double t) { return packer.pack_double(t); }

    inline packer& operator<<(packer &packer, const char *t) { return packer.pack_str(t); }

    typedef std::function<size_t(unsigned char*, size_t)> reader_t;

    class unpacker
    {
    public:
        reader_t m_reader;

        template<typename T>
        unpacker& unpack_float(T &t)
        {
            unsigned char head_byte=read_value<unsigned char>();

            switch(head_byte)
            {
                case byte_float32:
                    t=read_value<float>();
                    break;

                case byte_float64:
                    if(sizeof(T)<8){
                        throw std::exception("range check ?");
                    }
                    else{
                        t=read_value<double>();
                    }
                    break;

                default:
                    throw std::exception("not implemented. at " __FUNCTION__);
            }

            return *this;
        }

        template<typename T>
        unpacker &unpack_int(T &t)
        {
            unsigned char head_byte=read_value<unsigned char>();

            switch(head_byte)
            {
                case byte_uint8:
                    t=read_value<unsigned char>();
                    break;

                case byte_uint16:
                    if(sizeof(T)<2){
                        throw std::exception("range check ?");
                    }
                    else{
                        t=static_cast<T>(read_value<unsigned short>());
                    }
                    break;

                case byte_uint32:
                    if(sizeof(T)<4){
                        throw std::exception("range check ?");
                    }
                    else{
                        t=static_cast<T>(read_value<unsigned int>());
                    }
                    break;

                case byte_uint64:
                    if(sizeof(T)<8){
                        throw std::exception("range check ?");
                    }
                    else{
                        t=static_cast<T>(read_value<unsigned long long>());
                    }
                    break;

                case byte_int8:
                    t=static_cast<T>(read_value<char>());
                    break;

                case byte_int16:
                    if(sizeof(T)<2){
                        throw std::exception("range check ?");
                    }
                    else{
                        t=static_cast<T>(read_value<short>());
                    }
                    break;

                case byte_int32:
                    if(sizeof(T)<4){
                        throw std::exception("range check ?");
                    }
                    else {
                        t=static_cast<T>(read_value<int>());
                    }
                    break;

                case byte_int64:
                    if(sizeof(T)<8){
                        throw std::exception("range check ?");
                    }
                    else{
                        t=static_cast<T>(read_value<long long>());
                    }
                    break;

                default:
                    if(partial_bit_equal<positive_fixint>(head_byte)){
                        t=static_cast<char>(head_byte);
                    }
                    else if(partial_bit_equal<negative_fixint>(head_byte)){
                        t=-extract_head_byte<negative_fixint>(head_byte);
                    }
                    else{
                        throw std::exception("not implemented. at " __FUNCTION__);
                    }
                    break;
            }

            return *this;
        }

        unpacker& unpack_string(std::string &s)
        {
            unsigned char head_byte=read_value<unsigned char>();

            switch(head_byte)
            {
                case byte_str8:
                    {
                        auto len=read_value<unsigned char>();
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    break;

                case byte_str16:
                    {
                        auto len=read_value<unsigned short>();
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    break;

                case byte_str32:
                    {
                        auto len=read_value<unsigned int>();
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    break;

                default:
                    if(partial_bit_equal<fixstr>(head_byte)){
                        auto len=extract_head_byte<fixstr>(head_byte);
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    else{
                        throw std::exception("not implemented. at " __FUNCTION__);
                    }
                    break;
            }

            return *this;
        }

    private:
        template<typename T>
        T read_value()
        {
            T n;
            size_t size=read((unsigned char*)&n, sizeof(T));
            return n;
        }
            
        size_t read(unsigned char *p, size_t len)
        {
            size_t size=m_reader(p, len);
            assert(size==len);
            return size;
        }
    };

    inline unpacker& operator>>(unpacker &unpacker, char &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, short &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, int &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, long long &t) { return unpacker.unpack_int(t); }

    inline unpacker& operator>>(unpacker &unpacker, unsigned char &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned short &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned int &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned long long &t) { return unpacker.unpack_int(t); }

    inline unpacker& operator>>(unpacker &unpacker, float &t) { return unpacker.unpack_float(t); }
    inline unpacker& operator>>(unpacker &unpacker, double &t) { return unpacker.unpack_float(t); }

    inline unpacker& operator>>(unpacker &unpacker, std::string &t) { return unpacker.unpack_string(t); }

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

