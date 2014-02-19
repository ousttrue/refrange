#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <iterator>
#include <memory>
#include <type_traits>
#include <assert.h>


namespace mpack {
namespace msgpack {

    struct unpack_error: public std::invalid_argument
    {
        unpack_error(const std::string &message)
            : std::invalid_argument(message)
        {}
    };

    struct incompatible_unpack_type: public unpack_error
    {
        incompatible_unpack_type(const std::string &message)
            : unpack_error(message)
        {}
    };

    struct invalid_head_byte: public unpack_error
    {
        invalid_head_byte(const std::string &message)
            : unpack_error(message)
        {}
    };

    struct no_collection_tag{};
    struct array_tag{};
    struct map_tag{};

    template<class T>
        char extract_head_byte(unsigned char byte)
        {
            return static_cast<char>(byte & ~T::mask);
        }

    struct no_value_tag{};
    struct header_value_tag{};
    struct read_value_tag{};
    struct sequence_value_tag{};

    ///
    /// message types
    /// from https://github.com/msgpack/msgpack/blob/master/spec.md
    ///
    /// nil:
    /// +--------+
    /// |  0xc0  |
    /// +--------+
    struct nil_tag
    {
        enum bits_t { bits=0xc0 };
        typedef no_value_tag value_tag;
    };

    /// false:
    /// +--------+
    /// |  0xc2  |
    /// +--------+
    struct false_tag
    {
        enum bits_t { bits=0xc2 };
        typedef header_value_tag value_tag;

        typedef bool header_value_type;
        header_value_type value(){ return false; };
    };

    /// true:
    /// +--------+
    /// |  0xc3  |
    /// +--------+
    struct true_tag
    {
        enum bits_t { bits=0xc3 };
        typedef header_value_tag value_tag;

        typedef bool header_value_type;
        header_value_type value(){ return true; };
    };

    /// positive fixnum stores 7-bit positive integer
    /// +--------+
    /// |0XXXXXXX|
    /// +--------+
    struct positive_fixint_tag
    {
        enum bits_t { bits=0x00 };
        enum mask_t { mask=0x80 };
        typedef header_value_tag value_tag;

        unsigned char head_byte;
        positive_fixint_tag(unsigned char b)
            : head_byte(b)
        {}

        typedef char header_value_type;
        header_value_type value(){ return static_cast<header_value_type>(head_byte); };

        static bool is_match(unsigned char head_byte)
        {
            return bits == (mask & head_byte);
        }
    };

    /// negative fixnum stores 5-bit negative integer
    /// +--------+
    /// |111YYYYY|
    /// +--------+
    struct negative_fixint_tag
    {
        enum bits_t { bits=0xe0 };
        enum mask_t { mask=0xe0 };
        typedef header_value_tag value_tag;

        unsigned char head_byte;
        negative_fixint_tag(unsigned char b)
            : head_byte(b)
        {}

        typedef char header_value_type;
        header_value_type value(){ return -static_cast<header_value_type>(extract_head_byte<negative_fixint_tag>(head_byte)); };

        static bool is_match(unsigned char head_byte)
        {
            return bits == (mask & head_byte);
        }
    };

    /// uint 8 stores a 8-bit unsigned integer
    /// +--------+--------+
    /// |  0xcc  |ZZZZZZZZ|
    /// +--------+--------+
    struct uint8_tag
    {
        enum bits_t { bits=0xcc };
        typedef read_value_tag value_tag;
        typedef unsigned char read_type;
    };

    /// uint 16 stores a 16-bit big-endian unsigned integer
    /// +--------+--------+--------+
    /// |  0xcd  |ZZZZZZZZ|ZZZZZZZZ|
    /// +--------+--------+--------+
    struct uint16_tag
    {
        enum bits_t { bits=0xcd };
        typedef read_value_tag value_tag;
        typedef unsigned short read_type;
    };

    /// uint 32 stores a 32-bit big-endian unsigned integer
    /// +--------+--------+--------+--------+--------+
    /// |  0xce  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ
    /// +--------+--------+--------+--------+--------+
    struct uint32_tag
    {
        enum bits_t { bits=0xce };
        typedef read_value_tag value_tag;
        typedef unsigned int read_type;
    };

    /// uint 64 stores a 64-bit big-endian unsigned integer
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    /// |  0xcf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    struct uint64_tag
    {
        enum bits_t { bits=0xcf };
        typedef read_value_tag value_tag;
        typedef unsigned long long read_type;
    };

    /// int 8 stores a 8-bit signed integer
    /// +--------+--------+
    /// |  0xd0  |ZZZZZZZZ|
    /// +--------+--------+
    struct int8_tag
    {
        enum bits_t { bits=0xd0 };
        typedef read_value_tag value_tag;
        typedef char read_type;
    };

    /// int 16 stores a 16-bit big-endian signed integer
    /// +--------+--------+--------+
    /// |  0xd1  |ZZZZZZZZ|ZZZZZZZZ|
    /// +--------+--------+--------+
    struct int16_tag
    {
        enum bits_t { bits=0xd1 };
        typedef read_value_tag value_tag;
        typedef short read_type;
    };

    /// int 32 stores a 32-bit big-endian signed integer
    /// +--------+--------+--------+--------+--------+
    /// |  0xd2  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
    /// +--------+--------+--------+--------+--------+
    struct int32_tag
    {
        enum bits_t { bits=0xd2 };
        typedef read_value_tag value_tag;
        typedef int read_type;
    };

    /// int 64 stores a 64-bit big-endian signed integer
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    /// |  0xd3  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    struct int64_tag
    {
        enum bits_t { bits=0xd3 };
        typedef read_value_tag value_tag;
        typedef long long read_type;
    };

    /// float 32 stores a floating point number in IEEE 754 single precision floating point number format:
    /// +--------+--------+--------+--------+--------+
    /// |  0xca  |XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX
    /// +--------+--------+--------+--------+--------+
    struct float32_tag
    {
        enum bits_t { bits=0xca };
        typedef read_value_tag value_tag;
        typedef float read_type;
    };

    /// float 64 stores a floating point number in IEEE 754 double precision floating point number format:
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    /// |  0xcb  |YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
    struct float64_tag
    {
        enum bits_t { bits=0xcb };
        typedef read_value_tag value_tag;
        typedef double read_type;
    };

    /// fixstr stores a byte array whose length is upto 31 bytes:
    /// +--------+========+
    /// |101XXXXX|  data  |
    /// +--------+========+
    struct fixstr_tag
    {
        enum bits_t { bits=0xa0 };
        enum mask_t { mask=0xe0};
        typedef sequence_value_tag value_tag;

        unsigned char len;
        fixstr_tag(unsigned char l)
            : len(l)
        {}

        static bool is_match(unsigned char head_byte)
        {
            return bits == (mask & head_byte);
        }
    };

    /// str 8 stores a byte array whose length is upto (2^8)-1 bytes:
    /// +--------+--------+========+
    /// |  0xd9  |YYYYYYYY|  data  |
    /// +--------+--------+========+
    struct str8_tag
    {
        enum bits_t { bits=0xd9 };
        typedef sequence_value_tag value_tag;

        unsigned char len;
        str8_tag(unsigned char l)
            : len(l)
        {}
    };

    /// str 16 stores a byte array whose length is upto (2^16)-1 bytes:
    /// +--------+--------+--------+========+
    /// |  0xda  |ZZZZZZZZ|ZZZZZZZZ|  data  |
    /// +--------+--------+--------+========+
    struct str16_tag
    {
        enum bits_t { bits=0xda };
        typedef sequence_value_tag value_tag;

        unsigned short len;
        str16_tag(unsigned short l)
            : len(l)
        {}
    };

    /// str 32 stores a byte array whose length is upto (2^32)-1 bytes:
    /// +--------+--------+--------+--------+--------+========+
    /// |  0xdb  |AAAAAAAA|AAAAAAAA|AAAAAAAA|AAAAAAAA|  data  |
    /// +--------+--------+--------+--------+--------+========+
    struct str32_tag
    {
        enum bits_t { bits=0xdb };
        typedef sequence_value_tag value_tag;

        unsigned int len;
        str32_tag(unsigned int l)
            : len(l)
        {}
    };

    /// bin 8 stores a byte array whose length is upto (2^8)-1 bytes:
    /// +--------+--------+========+
    /// |  0xc4  |XXXXXXXX|  data  |
    /// +--------+--------+========+
    struct bin8_tag
    {
        enum bits_t { bits=0xc4 };
        typedef sequence_value_tag value_tag;

        unsigned char len;
        bin8_tag(unsigned char l)
            : len(l)
        {}
    };

    /// bin 16 stores a byte array whose length is upto (2^16)-1 bytes:
    /// +--------+--------+--------+========+
    /// |  0xc5  |YYYYYYYY|YYYYYYYY|  data  |
    /// +--------+--------+--------+========+
    struct bin16_tag
    {
        enum bits_t { bits=0xc5 };
        typedef sequence_value_tag value_tag;

        unsigned short len;
        bin16_tag(unsigned short l)
            : len(l)
        {}
    };

    /// bin 32 stores a byte array whose length is upto (2^32)-1 bytes:
    /// +--------+--------+--------+--------+--------+========+
    /// |  0xc6  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|  data  |
    /// +--------+--------+--------+--------+--------+========+
    struct bin32_tag
    {
        enum bits_t { bits=0xc6 };
        typedef sequence_value_tag value_tag;

        unsigned int len;
        bin32_tag(unsigned int l)
            : len(l)
        {}
    };

    /// fixarray stores an array whose length is upto 15 elements:
    /// +--------+~~~~~~~~~~~~~~~~~+
    /// |1001XXXX|    N objects    |
    /// +--------+~~~~~~~~~~~~~~~~~+
    struct fixarray_tag
    {
        enum is_array_t { is_array=1 };
        enum bits_t { bits=0x90 };
        enum mask_t { mask=0xf0 };
        typedef no_value_tag value_tag;

        unsigned char len;
        fixarray_tag(unsigned char l)
            : len(l)
        {}

        static bool is_match(unsigned char head_byte)
        {
            return bits == (mask & head_byte);
        }
    };

    /// array 16 stores an array whose length is upto (2^16)-1 elements:
    /// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
    /// |  0xdc  |YYYYYYYY|YYYYYYYY|    N objects    |
    /// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
    struct array16_tag
    {
        enum is_array_t { is_array=1 };
        enum bits_t { bits=0xdc };
        typedef no_value_tag value_tag;

        unsigned short len;
        array16_tag(unsigned short l)
            : len(l)
        {}
    };

    /// array 32 stores an array whose length is upto (2^32)-1 elements:
    /// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
    /// |  0xdd  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|    N objects    |
    /// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
    struct array32_tag
    {
        enum is_array_t { is_array=1 };
        enum bits_t { bits=0xdd };
        typedef no_value_tag value_tag;

        unsigned int len;
        array32_tag(unsigned int l)
            : len(l)
        {}
    };

    /// fixmap stores a map whose length is upto 15 elements
    /// +--------+~~~~~~~~~~~~~~~~~+
    /// |1000XXXX|   N*2 objects   |
    /// +--------+~~~~~~~~~~~~~~~~~+
    struct fixmap_tag
    {
        enum is_map_t { is_map=1 };
        enum bits_t { bits=0x80 };
        enum mask_t { mask=0xF0 };
        typedef no_value_tag value_tag;

        unsigned char len;
        fixmap_tag(unsigned char l)
            : len(l)
        {}

        static bool is_match(unsigned char head_byte)
        {
            return bits == (mask & head_byte);
        }
    };

    /// map 16 stores a map whose length is upto (2^16)-1 elements
    /// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
    /// |  0xde  |YYYYYYYY|YYYYYYYY|   N*2 objects   |
    /// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
    struct map16_tag
    {
        enum is_map_t { is_map=1 };
        enum bits_t { bits=0xde };
        typedef no_value_tag value_tag;

        unsigned short len;
        map16_tag(unsigned short l)
            : len(l)
        {}
    };

    /// map 32 stores a map whose length is upto (2^32)-1 elements
    /// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
    /// |  0xdf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|   N*2 objects   |
    /// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
    struct map32_tag
    {
        enum is_map_t { is_map=1 };
        enum bits_t { bits=0xdf };
        typedef no_value_tag value_tag;

        unsigned int len;
        map32_tag(unsigned int l)
            : len(l)
        {}
    };

#if 0
    /// fixext 1 stores an integer and a byte array whose length is 1 byte
    /// +--------+--------+--------+
    /// |  0xd4  |  type  |  data  |
    /// +--------+--------+--------+
    struct fixext1_tag
    {
        enum bits_t { bits=0xd4 };
    };

    /// fixext 2 stores an integer and a byte array whose length is 2 bytes
    /// +--------+--------+--------+--------+
    /// |  0xd5  |  type  |       data      |
    /// +--------+--------+--------+--------+
    struct fixext2_tag
    {
        enum bits_t { bits=0xd5 };
    };

    /// fixext 4 stores an integer and a byte array whose length is 4 bytes
    /// +--------+--------+--------+--------+--------+--------+
    /// |  0xd6  |  type  |                data               |
    /// +--------+--------+--------+--------+--------+--------+
    struct fixext4_tag
    {
        enum bits_t { bits=0xd6 };
    };

    /// fixext 8 stores an integer and a byte array whose length is 8 bytes
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
    /// |  0xd7  |  type  |                                  data                                 |
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
    struct fixext8_tag
    {
        enum bits_t { bits=0xd7 };
    };

    /// fixext 16 stores an integer and a byte array whose length is 16 bytes
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
    /// |  0xd8  |  type  |                                  data                                  
    /// +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
    /// +--------+--------+--------+--------+--------+--------+--------+--------+
    ///                               data (cont.)                              |
    /// +--------+--------+--------+--------+--------+--------+--------+--------+
    struct fixext16_tag
    {
        enum bits_t { bits=0xd8 };
    };

    /// ext 8 stores an integer and a byte array whose length is upto (2^8)-1 bytes:
    /// +--------+--------+--------+========+
    /// |  0xc7  |XXXXXXXX|  type  |  data  |
    /// +--------+--------+--------+========+
    struct ext8_tag
    {
        enum bits_t { bits=0xc7 };
    };

    /// ext 16 stores an integer and a byte array whose length is upto (2^16)-1 bytes:
    /// +--------+--------+--------+--------+========+
    /// |  0xc8  |YYYYYYYY|YYYYYYYY|  type  |  data  |
    /// +--------+--------+--------+--------+========+
    struct ext16_tag
    {
        enum bits_t { bits=0xc8 };
    };

    /// ext 32 stores an integer and a byte array whose length is upto (2^32)-1 bytes:
    /// +--------+--------+--------+--------+--------+--------+========+
    /// |  0xc9  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|  type  |  data  |
    /// +--------+--------+--------+--------+--------+--------+========+
    struct ext32_tag
    {
        enum bits_t { bits=0xc9 };
    };
#endif

    //////////////////////////////////////////////////////////////////////////////
    // packer
    //////////////////////////////////////////////////////////////////////////////
    template<class Tag, class Enable=void>
        struct collection_traits
        {
            typedef no_collection_tag tag;
        };

    template<class Tag>
        struct collection_traits<Tag, typename std::enable_if<Tag::is_map>::type>
        {
            typedef map_tag tag;
        };

    template<class Tag>
        struct collection_traits<Tag, typename std::enable_if<Tag::is_array>::type>
        {
            typedef array_tag tag;
        };


    typedef std::function<size_t(unsigned char*, size_t)> reader_t;
    struct collection_context
    {
        enum collection_t
        {
            collection_unknown,
            collection_array,
            collection_map,
        };
        collection_t type;
        size_t size;

        const unsigned char *p;
        size_t len;

        collection_context()
            : type(collection_unknown), size(0), p(0), len(0)
        {}

        collection_context(collection_t _type)
            : type(_type), size(0), p(0), len(0)
        {}

        collection_context(collection_t _type, size_t _size)
            : type(_type), size(_size), p(0), len(0)
        {}

        collection_context(collection_t _type, size_t _size, const unsigned char *_p, size_t _len)
            : type(_type), size(_size), p(_p), len(_len)
        {
        }
    };


    typedef std::function<size_t(const unsigned char*, size_t)> writer_t;
    typedef std::function<const unsigned char*(void)> getpointer_t;
    typedef std::function<size_t(void)> getsize_t;
    class packer
    {
        struct ItemCount
        {
            unsigned int current;
            unsigned int max;
            ItemCount()
                : current(0), max(0)
            {}
            ItemCount(unsigned int _max)
                : current(0), max(_max)
            {}
        };
        std::vector<ItemCount> m_nestItemCounts;

        writer_t m_writer;
        getpointer_t m_getpointer;
        getsize_t m_getsize;
        public:

        packer(const writer_t &writer, const getpointer_t &getpointer, const getsize_t &getsize)
            : m_writer(writer), m_getpointer(getpointer), m_getsize(getsize)
              , m_nestItemCounts(1)
        {}
        const unsigned char* pointer()const{ return m_getpointer(); }
        size_t size()const{ return m_getsize(); }

        size_t items()const{ return m_nestItemCounts[0].current; }
        void new_item() 
        { 
            auto &top=m_nestItemCounts.back();
            ++top.current;
            if(m_nestItemCounts.size()>1 && top.current==top.max){
                // close collection
                m_nestItemCounts.pop_back();
            }
            assert(!m_nestItemCounts.empty());
        }

        packer& pack_nil()
        {
            write_head_byte<nil_tag>();
            return *this;
        }

        packer& pack_bool(bool b)
        {
            if (b){
                write_head_byte<true_tag>();
            }
            else{
                write_head_byte<false_tag>();
            }
            return *this;
        }

        template<typename T>
            packer& pack_int(T n)
            {
                if(n<0){
                    if(n>-0x1f){
                        // negative fix int
                        new_item();
                        auto v = negative_fixint_tag::bits | -static_cast<char>(n);
                        write_value(static_cast<char>(v));
                        return *this;
                    }
                    if(n>-0xff){
                        // int8
                        write_head_byte<int8_tag>();
                        write_value(static_cast<char>(n));
                        return *this;
                    }
                    if(n>-0xffff){
                        // int16
                        write_head_byte<int16_tag>();
                        write_value(static_cast<short>(n));
                        return *this;
                    }
                    if(n>-((long long)0xffffffff)){
                        // int32
                        write_head_byte<int32_tag>();
                        write_value(static_cast<int>(n));
                        return *this;
                    }
                    else{
                        // int64
                        write_head_byte<int64_tag>();
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
                        write_head_byte<uint8_tag>();
                        write_value(static_cast<unsigned char>(n));
                        return *this;
                    }
                    if(n<=0xffff){
                        // uint16
                        write_head_byte<uint16_tag>();
                        write_value(static_cast<unsigned short>(n));
                        return *this;
                    }
                    else if(n<=0xffffffff){
                        // uint32
                        write_head_byte<uint32_tag>();
                        write_value(static_cast<unsigned int>(n));
                        return *this;
                    }
                    else{
                        // uint64
                        write_head_byte<uint64_tag>();
                        write_value(static_cast<unsigned long long>(n));
                        return *this;
                    }
                }
            }

        packer& pack_float(float n)
        {
            write_head_byte<float32_tag>();
            write_value(n);
            return *this;
        }

        packer& pack_double(double n)
        {
            write_head_byte<float64_tag>();
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
                auto v = fixstr_tag::bits | len;
                write_value(static_cast<char>(v));
                size_t size=write((const unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xff){
                // str8
                write_head_byte<str8_tag>();
                write_value(static_cast<unsigned char>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffff){
                // str16
                write_head_byte<str16_tag>();
                write_value(static_cast<unsigned short>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffffffff){
                // str32
                write_head_byte<str32_tag>();
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
                write_head_byte<bin8_tag>();
                write_value(static_cast<unsigned char>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffff){
                // bin16
                write_head_byte<bin16_tag>();
                write_value(static_cast<unsigned short>(len));
                size_t size=write((unsigned char*)p, len);
                assert(size==len);
            }
            else if(len<0xffffffff){
                // bin32
                write_head_byte<bin32_tag>();
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
            assert(c.type!=collection_context::collection_unknown);
            // allow empty collection
            //assert(c.size);

            if(c.type==collection_context::collection_array){
                if(c.size<0x0F){
                    // fixarray
                    new_item();
                    m_nestItemCounts.push_back(ItemCount(c.size));
                    auto v = fixarray_tag::bits | c.size;
                    write_value(static_cast<char>(v));
                }
                else if(c.size<0xFFFF){
                    // array16
                    write_head_byte<array16_tag>();
                    m_nestItemCounts.push_back(ItemCount(c.size));
                    write_value(static_cast<unsigned short>(c.size));
                }
                else if(c.size<0xFFFFFFFF){
                    // array32
                    write_head_byte<array32_tag>();
                    m_nestItemCounts.push_back(ItemCount(c.size));
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
                    m_nestItemCounts.push_back(ItemCount(c.size));
                    auto v = fixmap_tag::bits | pairs;
                    write_value(static_cast<char>(v));
                }
                else if(c.size<0xFFFF){
                    // map16
                    write_head_byte<map16_tag>();
                    m_nestItemCounts.push_back(ItemCount(c.size));
                    write_value(static_cast<unsigned short>(pairs));
                }
                else if(c.size<0xFFFFFFFF){
                    // map32
                    write_head_byte<map32_tag>();
                    m_nestItemCounts.push_back(ItemCount(c.size));
                    write_value(static_cast<unsigned int>(pairs));
                }
                else{
                    throw std::out_of_range(__FUNCTION__);
                }
            }
            else{
                throw std::exception(__FUNCTION__);
            }

            if(c.p){
                assert(m_nestItemCounts.back().max==c.size);
                m_nestItemCounts.pop_back();
                // close collection
                write(c.p, c.len);
            }

            return *this;
        }

        //private:
        template<class Tag>
            void write_head_byte()
            {
                new_item();
                write_value(static_cast<unsigned char>(Tag::bits));
            }

        template<typename T>
            void write_value(T n)
            {
                size_t size=write((unsigned char*)&n, sizeof(T));
                assert(size==sizeof(T));
            }

        size_t write(const unsigned char* p, size_t len)
        {
            return m_writer(p, len);
        }
    };

    // array
    inline collection_context array(size_t size=0){
        return collection_context(collection_context::collection_array, size);
    }
    inline collection_context array(const packer &packer)
    {
        return collection_context(collection_context::collection_array, 
                packer.items(), packer.pointer(), packer.size());
    }

    // map
    inline collection_context map(size_t size=0){
        return collection_context(collection_context::collection_map, size*2);
    }
    inline collection_context map(const packer &packer)
    {
        return collection_context(collection_context::collection_map, 
                packer.items(), packer.pointer(), packer.size());
    }

    // packer
    inline packer& operator<<(packer &p, packer &join) { 
        p.write(join.pointer(), join.size());
        return p;
    }

    // nil
    inline packer& operator<<(packer &packer, nil_tag nil) { return packer.pack_nil(); }

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
    // assign_traits
    struct nocopy_tag{};
    struct arithmetic_copy_tag{};
    struct sequence_tag{};

    template<typename Value, class ValueEnable=void>
        struct assign_traits
        {
            typedef nocopy_tag tag;
        };
    template<typename Value>
        struct assign_traits<Value
        , typename std::enable_if<std::is_arithmetic<Value>::value>::type
        >
        {
            typedef arithmetic_copy_tag tag;
        };
    template<>
        struct assign_traits<std::string>
        {
            typedef sequence_tag tag;
        };
    template<>
        struct assign_traits<std::vector<unsigned char>>
        {
            typedef sequence_tag tag;
        };


    //
    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, no_value_tag, arithmetic_copy_tag)
        {
            // do nothing
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, read_value_tag, arithmetic_copy_tag)
        {
            Tag::read_type t;
            auto size = reader((unsigned char*)&t, sizeof(t));
            assert(size == sizeof(t));

            *v = static_cast<Value>(t);
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, header_value_tag, arithmetic_copy_tag)
        {
            *v = static_cast<Value>(tag.value());
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, sequence_value_tag, arithmetic_copy_tag)
        {
            throw std::invalid_argument(__FUNCTION__);
        }

    //
    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, no_value_tag, nocopy_tag)
        {
            // do nothing
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, read_value_tag, nocopy_tag)
        {
            Tag::read_type t;
            auto size = reader((unsigned char*)&t, sizeof(t));
            assert(size == sizeof(t));

            // read only
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, header_value_tag, nocopy_tag)
        {
            // do nothing
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, sequence_value_tag, nocopy_tag)
        {
            if(tag.len==0){
                return;
            }
            std::vector<unsigned char> buf(tag.len);
            auto size=reader(&buf[0], buf.size());
            assert(size==buf.size());

            // read only
        }

    //
    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, no_value_tag, sequence_tag)
        {
            // no match type
            throw std::invalid_argument(__FUNCTION__);
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, read_value_tag, sequence_tag)
        {
            // no match type
            throw std::invalid_argument(__FUNCTION__);
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, header_value_tag, sequence_tag)
        {
            // no match type
            throw std::invalid_argument(__FUNCTION__);
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, Value *v, sequence_value_tag, sequence_tag)
        {
            if (tag.len == 0){
                return;
            }
            v->resize(tag.len);
            auto size=reader((unsigned char*)&((*v)[0]), tag.len);
            assert(size==tag.len);
        }


    template<typename Value>
        struct buffer
        {
            Value *m_p;

            buffer()
                : m_p(0)
            {}

            buffer(Value *p)
                : m_p(p)
            {
            }

            template<class Tag>
                void read_from(Tag &tag, reader_t &reader)
                {
                    read_and_assign(tag, reader,
                            m_p, 
                            Tag::value_tag(),
                            assign_traits<Value>::tag());
                }
        };


    template<typename Value>
        inline buffer<Value> create_buffer(Value &t)
        {
            return buffer<Value>(&t);
        }

    inline buffer<void> dummy_buffer()
    {
        return buffer<void>();
    }

    class unpacker
    {
        reader_t m_reader;
        // 0-255 (-1 is invalid)
        int m_peek_byte;
        public:

        unpacker(const reader_t &reader)
            : m_reader(reader), m_peek_byte(-1)
        {}

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

        unsigned char read_byte()
        {
            unsigned char c;
            read_value<unsigned char>(&c);
            return c;
        }

        unpacker& unpack_collection(collection_context &c)
        {
            auto head_byte=read_byte();
            switch(head_byte)
            {
                case array16_tag::bits:
                    {
                        unsigned short len;
                        read_value<unsigned short>(&len);
                        c.type=collection_context::collection_array;
                        c.size=len;
                    }
                    break;

                case array32_tag::bits:
                    {
                        unsigned int len;
                        read_value<unsigned int>(&len);
                        c.type=collection_context::collection_array;
                        c.size=len;
                    }
                    break;

                case map16_tag::bits:
                    {
                        unsigned short len;
                        read_value<unsigned short>(&len);
                        c.type=collection_context::collection_map;
                        c.size=len;
                    }
                    break;

                case map32_tag::bits:
                    {
                        unsigned int len;
                        read_value<unsigned int>(&len);
                        c.type=collection_context::collection_map;
                        c.size=len;
                    }
                    break;

                default:
                    if(fixarray_tag::is_match(head_byte)){
                        // collection
                        unsigned char len=extract_head_byte<fixarray_tag>(head_byte);
                        c.type=collection_context::collection_array;
                        c.size=len;
                    }
                    else if(fixmap_tag::is_match(head_byte)){
                        // collection
                        unsigned char len=extract_head_byte<fixmap_tag>(head_byte);
                        c.type=collection_context::collection_map;
                        c.size=len;
                    }
                    else{
                        throw unpack_error(__FUNCTION__);
                    }
                    break;
            }

            return *this;
        }

        unpacker& unpack_to_bool(bool &b)
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case true_tag::bits:
                    b=true;
                    break;

                case false_tag::bits:
                    b=false;
                    break;

                default:
                    throw incompatible_unpack_type(__FUNCTION__);
            }
            read_byte();
            return *this;
        }

        template<typename T>
            unpacker& unpack_to_arithmetic(T &t, typename std::enable_if<std::is_arithmetic<T>::value>::type* =0)
        {
            if(!is_arithmetic()){
                throw incompatible_unpack_type(__FUNCTION__);
            }

            auto b=create_buffer(t);
            auto head_byte=read_byte();
            switch(head_byte)
            {
                case float32_tag::bits:
                    b.read_from(float32_tag(), m_reader);
                    break;

                case float64_tag::bits:
                    b.read_from(float64_tag(), m_reader);
                    break;

                case uint8_tag::bits:
                    b.read_from(uint8_tag(), m_reader);
                    break;

                case uint16_tag::bits:
                    b.read_from(uint16_tag(), m_reader);
                    break;

                case uint32_tag::bits:
                    b.read_from(uint32_tag(), m_reader);
                    break;

                case uint64_tag::bits:
                    b.read_from(uint64_tag(), m_reader);
                    break;

                case int8_tag::bits:
                    b.read_from(int8_tag(), m_reader);
                    break;

                case int16_tag::bits:
                    b.read_from(int16_tag(), m_reader);
                    break;

                case int32_tag::bits:
                    b.read_from(int32_tag(), m_reader);
                    break;

                case int64_tag::bits:
                    b.read_from(int64_tag(), m_reader);
                    break;

                default:
                    if(positive_fixint_tag::is_match(head_byte)){
                        // char
                        b.read_from(positive_fixint_tag(head_byte), m_reader);
                    }
                    else if(negative_fixint_tag::is_match(head_byte)){
                        // uchar
                        b.read_from(negative_fixint_tag(head_byte), m_reader);
                    }
                    else{
                        throw invalid_head_byte(__FUNCTION__);
                    }
                    break;
            }

            return *this;
        }

        // todo: range
        template<typename T>
            unpacker& unpack_to_sequence(T &t)
            {
                if(!is_sequence()){
                    throw incompatible_unpack_type(__FUNCTION__);
                }

				auto b = create_buffer(t);

                auto head_byte=read_byte();
                switch(head_byte)
                {
                    case bin8_tag::bits:
                        {
                            unsigned char len;
                            read_value<unsigned char>(&len);
                            b.read_from(bin8_tag(len), m_reader);
                        }
                        break;

                    case bin16_tag::bits:
                        {
                            unsigned short len;
                            read_value<unsigned short>(&len);
                            b.read_from(bin16_tag(len), m_reader);
                        }
                        break;

                    case bin32_tag::bits:
                        {
                            unsigned int len;
                            read_value<unsigned int>(&len);
                            b.read_from(bin32_tag(len), m_reader);
                        }
                        break;

                    case str8_tag::bits:
                        {
                            unsigned char len;
                            read_value<unsigned char>(&len);
                            b.read_from(str8_tag(len), m_reader);
                        }
                        break;

                    case str16_tag::bits:
                        {
                            unsigned short len;
                            read_value<unsigned short>(&len);
                            b.read_from(str16_tag(len), m_reader);
                        }
                        break;

                    case str32_tag::bits:
                        {
                            unsigned int len;
                            read_value<unsigned int>(&len);
                            b.read_from(str32_tag(len), m_reader);
                        }
                        break;

                    default:
                        if(fixstr_tag::is_match(head_byte)){
                            auto len=extract_head_byte<fixstr_tag>(head_byte);
                            b.read_from(fixstr_tag(len), m_reader);
                        }
                        else {
                            throw invalid_head_byte(__FUNCTION__);
                        }
                        break;
                }

                return *this;
            }

        template<class BUFFER>
            unpacker& unpack(BUFFER &b)
            {
                auto head_byte=read_byte();
                switch(head_byte)
                {
                    case nil_tag::bits:
                        b.read_from(nil_tag(), m_reader);
                        break;

                    case false_tag::bits:
                        b.read_from(false_tag(), m_reader);
                        break;

                    case true_tag::bits:
                        b.read_from(true_tag(), m_reader);
                        break;

                    case float32_tag::bits:
                        b.read_from(float32_tag(), m_reader);
                        break;

                    case float64_tag::bits:
                        b.read_from(float64_tag(), m_reader);
                        break;

                    case uint8_tag::bits:
                        b.read_from(uint8_tag(), m_reader);
                        break;

                    case uint16_tag::bits:
                        b.read_from(uint16_tag(), m_reader);
                        break;

                    case uint32_tag::bits:
                        b.read_from(uint32_tag(), m_reader);
                        break;

                    case uint64_tag::bits:
                        b.read_from(uint64_tag(), m_reader);
                        break;

                    case int8_tag::bits:
                        b.read_from(int8_tag(), m_reader);
                        break;

                    case int16_tag::bits:
                        b.read_from(int16_tag(), m_reader);
                        break;

                    case int32_tag::bits:
                        b.read_from(int32_tag(), m_reader);
                        break;

                    case int64_tag::bits:
                        b.read_from(int64_tag(), m_reader);
                        break;

                        // sequence
                    case bin8_tag::bits:
                        {
                            unsigned char len;
                            read_value<unsigned char>(&len);
							b.read_from(bin8_tag(len), m_reader);
                        }
                        break;

                    case bin16_tag::bits:
                        {
                            unsigned short len;
                            read_value<unsigned short>(&len);
							b.read_from(bin16_tag(len), m_reader);
                        }
                        break;

                    case bin32_tag::bits:
                        {
                            unsigned int len;
                            read_value<unsigned int>(&len);
							b.read_from(bin32_tag(len), m_reader);
                        }
                        break;

                    case str8_tag::bits:
                        {
                            unsigned char len;
                            read_value<unsigned char>(&len);
							b.read_from(str8_tag(len), m_reader);
                        }
                        break;

                    case str16_tag::bits:
                        {
                            unsigned short len;
                            read_value<unsigned short>(&len);
							b.read_from(str16_tag(len), m_reader);
                        }
                        break;

                    case str32_tag::bits:
                        {
                            unsigned int len;
                            read_value<unsigned int>(&len);
							b.read_from(str32_tag(len), m_reader);
                        }
                        break;

                        // collection
                    case array16_tag::bits:
                    case array32_tag::bits:
                    case map16_tag::bits:
                    case map32_tag::bits:
                        throw unpack_error(__FUNCTION__);

#if 0
                    case ext8_tag::bits:
                    case ext16_tag::bits:
                    case ext32_tag::bits:
                    case fixext1_tag::bits:
                    case fixext2_tag::bits:
                    case fixext4_tag::bits:
                    case fixext8_tag::bits:
                    case fixext16_tag::bits:
                        // not implmented
                        throw std::exception(__FUNCTION__);
#endif

                    default:
                        if(positive_fixint_tag::is_match(head_byte)){
                            // char
                            b.read_from(positive_fixint_tag(head_byte), m_reader);
                        }
                        else if(negative_fixint_tag::is_match(head_byte)){
                            // uchar
                            b.read_from(negative_fixint_tag(head_byte), m_reader);
                        }
                        else if(fixstr_tag::is_match(head_byte)){
                            // str todo
                            auto len=extract_head_byte<fixstr_tag>(head_byte);
                            b.read_from(fixstr_tag(len), m_reader);
                        }
                        else {
                            throw std::invalid_argument(__FUNCTION__);
                        }
                        break;
                }

                return *this;
            }


        bool is_nil()
        {
            auto head_byte=peek_byte();
            return head_byte==nil_tag::bits; 
        }

        bool is_sigend()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case int8_tag::bits:
                case int16_tag::bits:
                case int32_tag::bits:
                case int64_tag::bits:
                    return true;
            }
            return positive_fixint_tag::is_match(head_byte);
        }

        bool is_unsigned()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case uint8_tag::bits:
                case uint16_tag::bits:
                case uint32_tag::bits:
                case uint64_tag::bits:
                    return true;
            }
            return negative_fixint_tag::is_match(head_byte);
        }

        bool is_float()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case float32_tag::bits:
                case float64_tag::bits:
                    return true;
            }
            return false;
        }

        bool is_arithmetic()
        {
            return is_sigend() || is_unsigned() || is_float();
        }

        bool is_bin()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case bin8_tag::bits:
                case bin16_tag::bits:
                case bin32_tag::bits:
                    return true;
            }
            return false;
        }

        bool is_str()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case str8_tag::bits:
                case str16_tag::bits:
                case str32_tag::bits:
                    return true;
            }
            return fixstr_tag::is_match(head_byte);
        }

        bool is_sequence()
        {
            return is_bin() || is_str();
        }

        bool is_array()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case array16_tag::bits:
                case array32_tag::bits:
                    return true;
            }
            return fixarray_tag::is_match(head_byte);
        }

        bool is_map()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case map16_tag::bits:
                case map32_tag::bits:
                    return true;
            }
            return fixmap_tag::is_match(head_byte);
        }

        bool is_integer()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case int8_tag::bits:
                case int16_tag::bits:
                case int32_tag::bits:
                case int64_tag::bits:
                case uint8_tag::bits:
                case uint16_tag::bits:
                case uint32_tag::bits:
                case uint64_tag::bits:
                    return true;
            }
            return positive_fixint_tag::is_match(head_byte)
                || negative_fixint_tag::is_match(head_byte);
        }

        private:
        template<typename T, typename W>
            void read_value(W *w)
            {
                T n;
                size_t size=read((unsigned char*)&n, sizeof(T));
                *w=n;
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
    inline unpacker& operator>>(unpacker &unpacker, bool &t) { return unpacker.unpack_to_bool(t); }
    // signed
    inline unpacker& operator>>(unpacker &unpacker, char &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, short &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, int &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, long long &t) { return unpacker.unpack_to_arithmetic(t); }
    // unsigned
    inline unpacker& operator>>(unpacker &unpacker, unsigned char &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned short &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned int &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned long long &t) { return unpacker.unpack_to_arithmetic(t); }
    // float
    inline unpacker& operator>>(unpacker &unpacker, float &t) { return unpacker.unpack_to_arithmetic(t); }
    inline unpacker& operator>>(unpacker &unpacker, double &t) { return unpacker.unpack_to_arithmetic(t); }
    // sequence
    inline unpacker& operator>>(unpacker &unpacker, std::string &t) { return unpacker.unpack_to_sequence(t); }
    inline unpacker& operator>>(unpacker &unpacker, std::vector<unsigned char> &t) { return unpacker.unpack_to_sequence(t); }

    // collection
    inline unpacker& operator>>(unpacker &unpacker, collection_context &c){ return unpacker.unpack_collection(c); }

    //////////////////////////////////////////////////////////////////////////////
    // utility
    //////////////////////////////////////////////////////////////////////////////
    inline packer create_external_vector_packer(std::vector<unsigned char> &packed_buffer)
    {
        auto buffer=&packed_buffer;
        auto writer=[buffer](const unsigned char *p, size_t size)->size_t
        {
            for(size_t i=0; i<size; ++i){
                buffer->push_back(*p++);
            }
            return size;
        };
        auto pointer=[buffer]()->const unsigned char *{
            return buffer->empty() ? 0 : &((*buffer)[0]);
        };
        auto size=[buffer]()->size_t{
            return buffer->size();
        };
        return packer(writer, pointer, size);
    }

    inline packer create_vector_packer()
    {
        auto buffer=std::make_shared<std::vector<unsigned char>>();
        auto writer=[buffer](const unsigned char *p, size_t size)->size_t
        {
            for(size_t i=0; i<size; ++i){
                buffer->push_back(*p++);
            }
            return size;
        };
        auto pointer=[buffer]()->const unsigned char *{
            return buffer->empty() ? 0 : &((*buffer)[0]);
        };
        auto size=[buffer]()->size_t{
            return buffer->size();
        };
        return packer(writer, pointer, size);
    }

    struct memory_unpacker
    {
        const unsigned char *m_begin;
        const unsigned char *m_p;
        size_t m_size;

        memory_unpacker(const unsigned char *begin, size_t size)
            : m_begin(begin), m_p(m_begin), m_size(size)
        {
        }

        size_t read(unsigned char *p, size_t size)
        {
            if(m_p+size>m_begin+m_size){
                throw std::range_error(__FUNCTION__);
            }

            std::copy(m_p, m_p+size, p);
            m_p+=size;
            return size;
        }
    };

    inline unpacker create_memory_unpacker(const unsigned char *begin, size_t len)
    {
        auto context=std::make_shared<memory_unpacker>(begin, len);
        auto reader=[context](unsigned char *p, size_t size)->size_t
        {
            return context->read(p, size);
        };
        return unpacker(reader);
    };

    inline unpacker create_unpacker_from_packer(packer &packer)
    {
        auto context=std::make_shared<memory_unpacker>(packer.pointer(), packer.size());
        auto reader=[context](unsigned char *p, size_t size)->size_t
        {
            return context->read(p, size);
        };
        return unpacker(reader);
    }

} // namespace
} // namespace

