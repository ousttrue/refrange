#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <stack>
#include <iterator>
#include <assert.h>


namespace mpack {
namespace msgpack {

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


    //////////////////////////////////////////////////////////////////////////////
    // packer
    //////////////////////////////////////////////////////////////////////////////
    typedef std::function<size_t(const unsigned char*, size_t)> writer_t;

    struct collection_context
    {
        enum collection_t
        {
            collection_unknown,
            collection_array,
            collection_map,
        };
        collection_t type;

        // if zero. collection is not closed. use self buffer
        // if non zero. collection is closed. use writer
        size_t size;
        writer_t writer;
        std::vector<unsigned char> buffer;
        bool closed;

        collection_context()
            : type(collection_unknown), size(0), closed(false)
        {}

        collection_context(collection_t _type)
            : type(_type), size(0), closed(false)
        {}

        collection_context(collection_t _type, size_t _size)
            : type(_type), size(_size), closed(size!=0)
        {}

        size_t write(const unsigned char *p, size_t len)
        {
            if(closed){
                return writer(p, len);
            }
            else{
                std::copy(p, p+len, std::back_inserter(buffer));
				return len;
            }
        }

        void close()
        {
            closed=true;
        }

        void new_item()
        {
            ++size;
        }
    };

    inline collection_context array(size_t size=0){
        return collection_context(collection_context::collection_array, size);
    }

    inline collection_context map(size_t size=0){
        // key + value
        return collection_context(collection_context::collection_map, size*2);
    }

    class packer
    {
        std::stack<collection_context> m_context_stack;

    public:
        writer_t m_writer;

        // increment collection member count
        void new_item()
        {
			if (m_context_stack.empty()){
				return;
			}
            m_context_stack.top().new_item();
        }

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
                    new_item();
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
                    new_item();
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
                new_item();
                auto v = fixstr::bits | len;
                write_value(static_cast<char>(v));
                size_t size=write((const unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xff){
                // str8
                write_head_byte(byte_str8);
                write_value(static_cast<unsigned char>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffff){
                // str16
                write_head_byte(byte_str16);
                write_value(static_cast<unsigned short>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffffffff){
                // str32
                write_head_byte(byte_str32);
                write_value(static_cast<unsigned int>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else{
                throw std::out_of_range(__FUNCTION__);
            }
            return *this;
        }

        packer& pack_bin(const unsigned char *p, size_t len)
        {
            if(len<0xff){
                // bin8
                write_head_byte(byte_bin8);
                write_value(static_cast<unsigned char>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffff){
                // bin16
                write_head_byte(byte_bin16);
                write_value(static_cast<unsigned short>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffffffff){
                // bin32
                write_head_byte(byte_bin32);
                write_value(static_cast<unsigned int>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else{
                throw std::out_of_range(__FUNCTION__);
            }
            return *this;
        }

        packer &begin_collection(const collection_context &c)
        {
            writer_t writer;
            if(m_context_stack.empty()){
                writer=m_writer;
            }
            else{
                auto &prev=m_context_stack.top();
                writer=[&prev](const unsigned char *p, size_t len)->size_t{
                    return prev.write(p, len);
                };
            }
            m_context_stack.push(c);
            m_context_stack.top().writer=writer;
            if(c.size==0){
                // do nothing. write when end_collection
            }
            else{
                write_collection_head(c);
            }

            return *this;
        }

        void write_collection_head(const collection_context &c)
        {
            if(c.type==collection_context::collection_array){
                if(c.size<0x0F){
                    // fixarray
                    new_item();
                    auto v = fixarray::bits | c.size;
                    write_value(static_cast<char>(v));
                }
                else if(c.size<0xFFFF){
                    // array16
                    write_head_byte(byte_array16);
                    write_value(static_cast<unsigned short>(c.size));
                }
                else if(c.size<0xFFFFFFFF){
                    // array32
                    write_head_byte(byte_array32);
                    write_value(static_cast<unsigned int>(c.size));
                }
                else{
                    throw std::out_of_range(__FUNCTION__);
                }
            }
            else if(c.type==collection_context::collection_map){
                assert(c.size%2==0);
                auto pairs=c.size/2;
                if(c.size<0x0F){
                    // fixmap
                    new_item();
                    auto v = fixmap::bits | pairs;
                    write_value(static_cast<char>(v));
                }
                else if(c.size<0xFFFF){
                    // map16
                    write_head_byte(byte_map16);
                    write_value(static_cast<unsigned short>(pairs));
                }
                else if(c.size<0xFFFFFFFF){
                    // map32
                    write_head_byte(byte_map32);
                    write_value(static_cast<unsigned int>(pairs));
                }
                else{
                    throw std::out_of_range(__FUNCTION__);
                }
            }
            else{
                throw std::exception(__FUNCTION__);
            }
        }

        packer& end_collection()
        {
            m_context_stack.top().close();
            write_collection_head(m_context_stack.top());

            // copy buffer
            auto buffer=m_context_stack.top().buffer;
            m_context_stack.pop();
            if(!buffer.empty()){
                write(&buffer[0], buffer.size());
            }

            return *this;
        }

    private:
        void write_head_byte(byte_type head_byte)
        {
            new_item();
            write_value(static_cast<unsigned char>(head_byte));
        }

        template<typename T>
        void write_value(T n)
        {
            size_t size=write((unsigned char*)&n, sizeof(T));
            assert(size==sizeof(T));
        }

        size_t write(const unsigned char* p, size_t len)
        {
            if(m_context_stack.empty()){
                return m_writer(p, len);
            }
            else{
                m_context_stack.top().write(p, len);
            }
        }
    };

    // bool
    inline packer& operator<<(packer &packer, bool t) { return packer.pack_bool(t); }

    // signed
    inline packer& operator<<(packer &packer, char t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, short t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, int t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, long long t) { return packer.pack_int(t); }

    // unsigned
    inline packer& operator<<(packer &packer, unsigned char t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, unsigned short t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, unsigned int t) { return packer.pack_int(t); }
    inline packer& operator<<(packer &packer, unsigned long long t) { return packer.pack_int(t); }

    // float
    inline packer& operator<<(packer &packer, const float t) { return packer.pack_float(t); }
    inline packer& operator<<(packer &packer, const double t) { return packer.pack_double(t); }

    // str
    inline packer& operator<<(packer &packer, const char *t) { return packer.pack_str(t); }
    inline packer& operator<<(packer &packer, const std::string &t){ return packer.pack_str(t.c_str()); }

    // bin
    inline packer& operator<<(packer &packer, const std::vector<unsigned char> &t){ 
        if(!t.empty()){ packer.pack_bin(&t[0], t.size()); }; return packer;
    }

    // collection
    inline packer& operator<<(packer &packer, const collection_context &t){ return packer.begin_collection(t); }

    //////////////////////////////////////////////////////////////////////////////
    // unpacker
    //////////////////////////////////////////////////////////////////////////////
    typedef std::function<size_t(unsigned char*, size_t)> reader_t;

    class unpacker
    {
    public:
        reader_t m_reader;
        // 0-255 (-1 is invalid)
        int m_peek_byte;

        unpacker()
            : m_peek_byte(-1)
        {
        }

        unpacker& unpack_bool(bool &t)
        {
            unsigned char head_byte=read_value<unsigned char>();
            t=(head_byte==byte_true);
            return *this;
        }

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
                        s.reserve(len);
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    break;

                case byte_str16:
                    {
                        auto len=read_value<unsigned short>();
                        s.reserve(len);
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    break;

                case byte_str32:
                    {
                        auto len=read_value<unsigned int>();
                        s.reserve(len);
                        for(size_t i=0; i<len; ++i){
                            s.push_back(read_value<char>());
                        }
                    }
                    break;

                default:
                    if(partial_bit_equal<fixstr>(head_byte)){
                        auto len=extract_head_byte<fixstr>(head_byte);
                        s.reserve(len);
                        for(int i=0; i<len; ++i){
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

        unpacker& unpack_bin(std::vector<unsigned char> &b)
        {
            unsigned char head_byte=read_value<unsigned char>();

            switch(head_byte)
            {
                case byte_bin8:
                    {
                        auto len=read_value<unsigned char>();
                        b.reserve(len);
                        for(size_t i=0; i<len; ++i){
                            b.push_back(read_value<char>());
                        }
                    }
                    break;

                case byte_bin16:
                    {
                        auto len=read_value<unsigned short>();
                        b.reserve(len);
                        for(size_t i=0; i<len; ++i){
                            b.push_back(read_value<char>());
                        }
                    }
                    break;

                case byte_bin32:
                    {
                        auto len=read_value<unsigned int>();
                        b.reserve(len);
                        for(size_t i=0; i<len; ++i){
                            b.push_back(read_value<char>());
                        }
                    }
                    break;

                default:
                    throw std::exception("not implemented. at " __FUNCTION__);
            }

            return *this;
        }

        bool is_array()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case byte_array16:
                case byte_array32:
                    return true;
            }
            return partial_bit_equal<fixarray>(head_byte);
        }

        bool is_map()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case byte_map16:
                case byte_map32:
                    return true;
            }
            return partial_bit_equal<fixmap>(head_byte);
        }

        bool is_integer()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case byte_int8:
                case byte_int16:
                case byte_int32:
                case byte_int64:
                case byte_uint8:
                case byte_uint16:
                case byte_uint32:
                case byte_uint64:
                    return true;
            }
            return partial_bit_equal<positive_fixint>(head_byte)
                || partial_bit_equal<negative_fixint>(head_byte);
        }

        unpacker &unpack_collection(collection_context &c)
        {
            auto head_byte=read_value<unsigned char>();
            switch(head_byte)
            {
                case byte_array16:
                    c.type=collection_context::collection_array;
                    c.size=read_value<unsigned short>();
                    break;

                case byte_array32:
                    c.type=collection_context::collection_array;
                    c.size=read_value<unsigned int>();
                    break;

                case byte_map16:
                    c.type=collection_context::collection_map;
                    c.size=read_value<unsigned short>();
                    break;

                case byte_map32:
                    c.type=collection_context::collection_map;
                    c.size=read_value<unsigned int>();
                    break;

                default:
                    if(partial_bit_equal<fixarray>(head_byte)){
                        c.type=collection_context::collection_array;
                        c.size=extract_head_byte<fixarray>(head_byte);
                    }
                    else if(partial_bit_equal<fixmap>(head_byte)){
                        c.type=collection_context::collection_map;
                        c.size=extract_head_byte<fixmap>(head_byte);
                    }
                    else{
						throw std::invalid_argument(__FUNCTION__);
                    }
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

        unsigned char peek_byte()
        {
            if(m_peek_byte==-1){
                unsigned char byte;
                auto size=read(&byte, 1);
                assert(size==1);
                m_peek_byte=byte;
            }
            return m_peek_byte;
        }
            
        size_t read(unsigned char *p, size_t len)
        {
            if(m_peek_byte==-1){
                size_t size=m_reader(p, len);
                assert(size==len);
                return size;
            }
            else{
                {
                    // first peek byte
                    *p=static_cast<unsigned char>(m_peek_byte);
                    m_peek_byte=-1;
                }
                size_t size=m_reader(p+1, len-1);
                ++size;
                assert(size==len);
                return size;
            }
        }
    };

    // bool
    inline unpacker& operator>>(unpacker &unpacker, bool &t) { return unpacker.unpack_bool(t); }

    // signed
    inline unpacker& operator>>(unpacker &unpacker, char &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, short &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, int &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, long long &t) { return unpacker.unpack_int(t); }

    // unsigned
    inline unpacker& operator>>(unpacker &unpacker, unsigned char &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned short &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned int &t) { return unpacker.unpack_int(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned long long &t) { return unpacker.unpack_int(t); }

    // float
    inline unpacker& operator>>(unpacker &unpacker, float &t) { return unpacker.unpack_float(t); }
    inline unpacker& operator>>(unpacker &unpacker, double &t) { return unpacker.unpack_float(t); }

    // str
    inline unpacker& operator>>(unpacker &unpacker, std::string &t) { return unpacker.unpack_string(t); }

    // bin
    inline unpacker& operator>>(unpacker &unpacker, std::vector<unsigned char> &t) { return unpacker.unpack_bin(t); }

    // array
    inline unpacker& operator>>(unpacker &unpacker, collection_context &t){ return unpacker.unpack_collection(t); }

    //////////////////////////////////////////////////////////////////////////////
    // utility
    //////////////////////////////////////////////////////////////////////////////
    class vector_packer: public packer
    {
        std::vector<unsigned char> &m_packed_buffer;

        vector_packer(const vector_packer&);
        vector_packer& operator=(const vector_packer&); 
    public:
        vector_packer(std::vector<unsigned char> &packed_buffer)
            : m_packed_buffer(packed_buffer)
        {
			auto buffer=&m_packed_buffer;
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

} // namespace
} // namespace

