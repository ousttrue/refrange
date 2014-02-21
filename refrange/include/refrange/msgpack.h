#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <iterator>
#include <memory>
#include <type_traits>
#include <assert.h>
#include "reader.h"

namespace refrange {
namespace msgpack {


//////////////////////////////////////////////////////////////////////////////
// exception
//////////////////////////////////////////////////////////////////////////////
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


// read_category
struct no_read_tag{};
struct read_value_tag{};

// value_category
struct no_value_tag{};
struct single_value_tag{};
struct sequence_value_tag{};

struct base_tag
{
    const unsigned char *begin;

    base_tag(const unsigned char *b)
        : begin(b)
    {}
};

struct header_value_base_tag: public base_tag
{
    typedef no_read_tag read_category;
    typedef single_value_tag value_category;

    header_value_base_tag(const unsigned char *begin)
        : base_tag(begin)
    {}
};

struct read_single_value_base_tag: public base_tag
{
    typedef read_value_tag read_category;
    typedef single_value_tag value_category;

    read_single_value_base_tag(const unsigned char *begin)
        : base_tag(begin)
    {}
};

struct read_sequence_value_base_tag: public base_tag
{
    typedef read_value_tag read_category;
    typedef sequence_value_tag value_category;

    read_sequence_value_base_tag(const unsigned char *begin)
        : base_tag(begin)
    {}
};

struct no_value_base_tag: public base_tag
{
    typedef no_read_tag read_category;
    typedef no_value_tag value_category;

    no_value_base_tag(const unsigned char *begin)
        : base_tag(begin)
    {}
};


///
/// message types
/// from https://github.com/msgpack/msgpack/blob/master/spec.md
///
/// nil:
/// +--------+
/// |  0xc0  |
/// +--------+
struct nil_tag: public no_value_base_tag
{
    enum bits_t { bits=0xc0 };

    nil_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// false:
/// +--------+
/// |  0xc2  |
/// +--------+
struct false_tag: public header_value_base_tag
{
    enum bits_t { bits=0xc2 };
    bool value(){ return false; };

    false_tag(const unsigned char *begin)
        : header_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// true:
/// +--------+
/// |  0xc3  |
/// +--------+
struct true_tag: public header_value_base_tag
{
    enum bits_t { bits=0xc3 };
    bool value(){ return true; };

    true_tag(const unsigned char *begin)
        : header_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// positive fixnum stores 7-bit positive integer
/// +--------+
/// |0XXXXXXX|
/// +--------+
struct positive_fixint_tag: public header_value_base_tag
{
    enum bits_t { bits=0x00 };
    enum mask_t { mask=0x80 };

    positive_fixint_tag(const unsigned char *begin)
        : header_value_base_tag(begin)
    {
        assert(is_match(*begin));
    }

    typedef char header_value_type;
    header_value_type value(){ return static_cast<header_value_type>(*begin); };

    static unsigned char extract_head_byte(unsigned char head_byte)
    {
        return static_cast<unsigned char>(head_byte & ~mask);
    }

    static bool is_match(unsigned char head_byte)
    {
        return bits == (mask & head_byte);
    }
};

/// negative fixnum stores 5-bit negative integer
/// +--------+
/// |111YYYYY|
/// +--------+
struct negative_fixint_tag: public header_value_base_tag
{
    enum bits_t { bits=0xe0 };
    enum mask_t { mask=0xe0 };

    negative_fixint_tag(const unsigned char *begin)
        : header_value_base_tag(begin)
    {
        assert(is_match(*begin));
    }

    typedef char header_value_type;
    header_value_type value(){ return -static_cast<header_value_type>(extract_head_byte(*begin)); };

    static unsigned char extract_head_byte(unsigned char head_byte)
    {
        return static_cast<unsigned char>(head_byte & ~mask);
    }

    static bool is_match(unsigned char head_byte)
    {
        return bits == (mask & head_byte);
    }
};

/// uint 8 stores a 8-bit unsigned integer
/// +--------+--------+
/// |  0xcc  |ZZZZZZZZ|
/// +--------+--------+
struct uint8_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xcc };
    typedef read_value_tag value_tag;
    typedef unsigned char read_type;

    uint8_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// uint 16 stores a 16-bit big-endian unsigned integer
/// +--------+--------+--------+
/// |  0xcd  |ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+
struct uint16_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xcd };
    typedef read_value_tag value_tag;
    typedef unsigned short read_type;

    uint16_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// uint 32 stores a 32-bit big-endian unsigned integer
/// +--------+--------+--------+--------+--------+
/// |  0xce  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ
/// +--------+--------+--------+--------+--------+
struct uint32_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xce };
    typedef read_value_tag value_tag;
    typedef unsigned int read_type;

    uint32_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// uint 64 stores a 64-bit big-endian unsigned integer
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
/// |  0xcf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
struct uint64_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xcf };
    typedef read_value_tag value_tag;
    typedef unsigned long long read_type;

    uint64_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// int 8 stores a 8-bit signed integer
/// +--------+--------+
/// |  0xd0  |ZZZZZZZZ|
/// +--------+--------+
struct int8_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xd0 };
    typedef read_value_tag value_tag;
    typedef char read_type;

    int8_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// int 16 stores a 16-bit big-endian signed integer
/// +--------+--------+--------+
/// |  0xd1  |ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+
struct int16_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xd1 };
    typedef read_value_tag value_tag;
    typedef short read_type;

    int16_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// int 32 stores a 32-bit big-endian signed integer
/// +--------+--------+--------+--------+--------+
/// |  0xd2  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+--------+--------+
struct int32_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xd2 };
    typedef read_value_tag value_tag;
    typedef int read_type;

    int32_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// int 64 stores a 64-bit big-endian signed integer
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
/// |  0xd3  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
struct int64_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xd3 };
    typedef read_value_tag value_tag;
    typedef long long read_type;

    int64_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// float 32 stores a floating point number in IEEE 754 single precision floating point number format:
/// +--------+--------+--------+--------+--------+
/// |  0xca  |XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX
/// +--------+--------+--------+--------+--------+
struct float32_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xca };
    typedef read_value_tag value_tag;
    typedef float read_type;

    float32_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// float 64 stores a floating point number in IEEE 754 double precision floating point number format:
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
/// |  0xcb  |YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|YYYYYYYY|
/// +--------+--------+--------+--------+--------+--------+--------+--------+--------+
struct float64_tag: public read_single_value_base_tag
{
    enum bits_t { bits=0xcb };
    typedef read_value_tag value_tag;
    typedef double read_type;

    float64_tag(const unsigned char *begin)
        : read_single_value_base_tag(begin)
    {
        assert(*begin==bits);
    }
};

/// fixstr stores a byte array whose length is upto 31 bytes:
/// +--------+========+
/// |101XXXXX|  data  |
/// +--------+========+
struct fixstr_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xa0 };
    enum mask_t { mask=0xe0};

    fixstr_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned char len()const
    {
        return extract_head_byte(*begin);
    }

    static unsigned char extract_head_byte(unsigned char head_byte)
    {
        return static_cast<unsigned char>(head_byte & ~mask);
    }

    static bool is_match(unsigned char head_byte)
    {
        return bits == (mask & head_byte);
    }
};

/// str 8 stores a byte array whose length is upto (2^8)-1 bytes:
/// +--------+--------+========+
/// |  0xd9  |YYYYYYYY|  data  |
/// +--------+--------+========+
struct str8_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xd9 };
    typedef sequence_value_tag value_tag;

    str8_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned char len()const
    {
        return *(begin+1);
    }
};

/// str 16 stores a byte array whose length is upto (2^16)-1 bytes:
/// +--------+--------+--------+========+
/// |  0xda  |ZZZZZZZZ|ZZZZZZZZ|  data  |
/// +--------+--------+--------+========+
struct str16_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xda };
    typedef sequence_value_tag value_tag;

    str16_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned short len()const
    {
        return *((unsigned short*)(begin+1));
    }
};

/// str 32 stores a byte array whose length is upto (2^32)-1 bytes:
/// +--------+--------+--------+--------+--------+========+
/// |  0xdb  |AAAAAAAA|AAAAAAAA|AAAAAAAA|AAAAAAAA|  data  |
/// +--------+--------+--------+--------+--------+========+
struct str32_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xdb };
    typedef sequence_value_tag value_tag;

    str32_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned int len()const
    {
        return *((unsigned int*)(begin+1));
    }
};

/// bin 8 stores a byte array whose length is upto (2^8)-1 bytes:
/// +--------+--------+========+
/// |  0xc4  |XXXXXXXX|  data  |
/// +--------+--------+========+
struct bin8_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xc4 };
    typedef sequence_value_tag value_tag;

    bin8_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned char len()const
    {
        return *(begin+1);
    }
};

/// bin 16 stores a byte array whose length is upto (2^16)-1 bytes:
/// +--------+--------+--------+========+
/// |  0xc5  |YYYYYYYY|YYYYYYYY|  data  |
/// +--------+--------+--------+========+
struct bin16_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xc5 };
    typedef sequence_value_tag value_tag;

    bin16_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned short len()const
    {
        return *((unsigned short*)(begin+1));
    }
};

/// bin 32 stores a byte array whose length is upto (2^32)-1 bytes:
/// +--------+--------+--------+--------+--------+========+
/// |  0xc6  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|  data  |
/// +--------+--------+--------+--------+--------+========+
struct bin32_tag: public read_sequence_value_base_tag
{
    enum bits_t { bits=0xc6 };
    typedef sequence_value_tag value_tag;

    bin32_tag(const unsigned char *begin)
        : read_sequence_value_base_tag(begin)
    {}

    unsigned int len()const
    {
        return *((unsigned int*)(begin+1));
    }
};

/// fixarray stores an array whose length is upto 15 elements:
/// +--------+~~~~~~~~~~~~~~~~~+
/// |1001XXXX|    N objects    |
/// +--------+~~~~~~~~~~~~~~~~~+
struct fixarray_tag: public no_value_base_tag
{
    enum is_array_t { is_array=1 };
    enum bits_t { bits=0x90 };
    enum mask_t { mask=0xf0 };

    fixarray_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {}

    static unsigned char extract_head_byte(unsigned char head_byte)
    {
        return static_cast<unsigned char>(head_byte & ~mask);
    }

    static bool is_match(unsigned char head_byte)
    {
        return bits == (mask & head_byte);
    }

    unsigned char len()const
    {
        return extract_head_byte(*begin);
    }
};

/// array 16 stores an array whose length is upto (2^16)-1 elements:
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xdc  |YYYYYYYY|YYYYYYYY|    N objects    |
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
struct array16_tag: public no_value_base_tag
{
    enum is_array_t { is_array=1 };
    enum bits_t { bits=0xdc };

    array16_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {}

    unsigned short len()const
    {
        return *((unsigned short*)(begin+1));
    }
};

/// array 32 stores an array whose length is upto (2^32)-1 elements:
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xdd  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|    N objects    |
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
struct array32_tag: public no_value_base_tag
{
    enum is_array_t { is_array=1 };
    enum bits_t { bits=0xdd };
    typedef no_value_tag value_tag;

    array32_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {}

    unsigned int len()const
    {
        return *((unsigned int*)(begin+1));
    }
};

/// fixmap stores a map whose length is upto 15 elements
/// +--------+~~~~~~~~~~~~~~~~~+
/// |1000XXXX|   N*2 objects   |
/// +--------+~~~~~~~~~~~~~~~~~+
struct fixmap_tag: public no_value_base_tag
{
    enum is_map_t { is_map=1 };
    enum bits_t { bits=0x80 };
    enum mask_t { mask=0xF0 };
    typedef no_value_tag value_tag;

    fixmap_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {}

    static unsigned char extract_head_byte(unsigned char head_byte)
    {
        return static_cast<unsigned char>(head_byte & ~mask);
    }

    static bool is_match(unsigned char head_byte)
    {
        return bits == (mask & head_byte);
    }

    unsigned char len()const
    {
        return extract_head_byte(*begin);
    }
};

/// map 16 stores a map whose length is upto (2^16)-1 elements
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xde  |YYYYYYYY|YYYYYYYY|   N*2 objects   |
/// +--------+--------+--------+~~~~~~~~~~~~~~~~~+
struct map16_tag: public no_value_base_tag
{
    enum is_map_t { is_map=1 };
    enum bits_t { bits=0xde };
    typedef no_value_tag value_tag;

    map16_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {}

    unsigned short len()const
    {
        return *((unsigned short*)(begin+1));
    }
};

/// map 32 stores a map whose length is upto (2^32)-1 elements
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
/// |  0xdf  |ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|ZZZZZZZZ|   N*2 objects   |
/// +--------+--------+--------+--------+--------+~~~~~~~~~~~~~~~~~+
struct map32_tag: public no_value_base_tag
{
    enum is_map_t { is_map=1 };
    enum bits_t { bits=0xdf };
    typedef no_value_tag value_tag;

    map32_tag(const unsigned char *begin)
        : no_value_base_tag(begin)
    {}

    unsigned int len()const
    {
        return *((unsigned int*)(begin+1));
    }
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
// buffer
//////////////////////////////////////////////////////////////////////////////
struct base_buffer
{
    template<class Tag>
        void read_from(Tag &tag, range_reader &reader)
        {
            // read only
            _read_from(tag, reader, Tag::read_category(), Tag::value_category());
        }

private:
    // read_category
    // * no_read_tag
    // * read_value_tag
    //
    // value_category
    // * no_value_tag
    // * single_value_tag
    // * sequence_value_tag
    //
    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, single_value_tag)
        {
            // do nothing
        }
    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, sequence_value_tag)
        {
            // do nothing
        }
    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, no_value_tag)
        {
            // do nothing
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, single_value_tag)
        {
            // drop value
            Tag::read_type t;            
            auto size=reader.read((unsigned char*)&t, sizeof(Tag::read_type));
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, sequence_value_tag)
        {
            // drop value
			auto len=tag.len();
            for(size_t i=0; i<len; ++i){
                unsigned char c;
                auto size=reader.read(&c, sizeof(c));
            }
        }
};

struct collection_context: public base_buffer
{
    enum collection_t
    {
        collection_unknown,
        collection_array,
        collection_map,
    };
    collection_t type;
    size_t size;

    template<typename T>
        struct collection_traits
        {
            static const int value=collection_unknown;
        };
    // array
    template<>
        struct collection_traits<fixarray_tag>
        {
            static const int value=collection_array;
        };
    template<>
        struct collection_traits<array16_tag>
        {
            static const int value=collection_array;
        };
    template<>
        struct collection_traits<array32_tag>
        {
            static const int value=collection_array;
        };
    // map
    template<>
        struct collection_traits<fixmap_tag>
        {
            static const int value=collection_map;
        };
    template<>
        struct collection_traits<map16_tag>
        {
            static const int value=collection_map;
        };
    template<>
        struct collection_traits<map32_tag>
        {
            static const int value=collection_map;
        };

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

    template<class Tag>
        void read_from(Tag &tag, range_reader &reader
                , typename std::enable_if<collection_traits<Tag>::value==collection_unknown>::type* =0)
        {
            base_buffer::read_from(tag, reader);
        }

    template<class Tag>
        void read_from(Tag &tag, range_reader &reader
                , typename std::enable_if<collection_traits<Tag>::value==collection_array>::type* =0)
        {
            type=collection_array;
            size=tag.len();
        }

    template<class Tag>
        void read_from(Tag &tag, range_reader &reader
                , typename std::enable_if<collection_traits<Tag>::value==collection_map>::type* =0)
        {
            type=collection_map;
            size=tag.len();
        }
};

template<typename Value
>
struct arithmetic_buffer: public base_buffer
{
    Value &m_v;

    arithmetic_buffer(Value &v)
        : m_v(v)
    {
    }

    template<class Tag
        >
        void read_from(Tag &tag, range_reader &reader
                , typename std::enable_if<std::is_arithmetic<Value>::value>::type* =0)
        {
            _read_from(tag, reader, Tag::read_category(), Tag::value_category());
        }

private:
    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, no_value_tag)
        {
            // error ?
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, single_value_tag)
        {
            // header value
            m_v=static_cast<Value>(tag.value());
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, single_value_tag)
        {
            Tag::read_type t;            
            auto size=reader.read((unsigned char*)&t, sizeof(Tag::read_type));
            m_v=static_cast<Value>(t);
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, sequence_value_tag)
        {
            // error ?
        }
};


// avoid warning
struct bool_buffer: public base_buffer
{
    bool &m_v;

    bool_buffer(bool &v)
        : m_v(v)
    {
    }

    template<class Tag
        >
        void read_from(Tag &tag, range_reader &reader
                , typename std::enable_if<std::is_arithmetic<bool>::value>::type* =0)
        {
            _read_from(tag, reader, Tag::read_category(), Tag::value_category());
        }

private:
    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, no_value_tag)
        {
            // error ?
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, single_value_tag)
        {
            // header value
            m_v=tag.value()!=0;
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, single_value_tag)
        {
            Tag::read_type t;            
			auto size=reader.read((unsigned char*)&t, sizeof(Tag::read_type));
            m_v=t!=0;
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, sequence_value_tag)
        {
            // error ?
        }
};


struct byte_range_buffer: public base_buffer
{
    range_reader &m_range;

    byte_range_buffer(range_reader &range)
        : m_range(range)
    {
    }

    template<class Tag
        >
        void read_from(Tag &tag, range_reader &reader)
        {
			// advance reader
            base_buffer::read_from(tag, reader);

            m_range=range_reader(tag.begin, reader.current());
        }
};


template<typename Value>
struct sequence_buffer: public base_buffer
{
    Value &m_v;

    sequence_buffer(Value &v)
        : m_v(v)
    {
    }

    template<class Tag>
        void read_from(Tag &tag, range_reader &reader)
        {
            _read_from(tag, reader, Tag::read_category(), Tag::value_category());
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, no_value_tag)
        {
            // error ?
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, no_read_tag, single_value_tag)
        {
            // error ?
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, single_value_tag)
        {
            // error ?
        }

    template<class Tag>
        void _read_from(Tag &tag, range_reader &reader, read_value_tag, sequence_value_tag)
        {
			auto len=tag.len();
            if(len==0){
                return;
            }
            m_v.resize(len);
            auto size=reader.read((unsigned char*)&m_v[0], m_v.size());
            assert(size==m_v.size());
        }
};


//////////////////////////////////////////////////////////////////////////////
// packer
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
// unpacker
//////////////////////////////////////////////////////////////////////////////
template<typename Value
    >
inline base_buffer create_buffer(Value &t
        , typename std::enable_if<!std::is_arithmetic<Value>::value>::type* =0
        )
{
    return base_buffer();
}

template<typename Value
    >
inline arithmetic_buffer<Value> create_buffer(Value &t
        , typename std::enable_if<std::is_arithmetic<Value>::value>::type* =0
        )
{
    return arithmetic_buffer<Value>(t);
}

inline bool_buffer create_buffer(bool &t)
{
    return bool_buffer(t);
}

inline byte_range_buffer create_buffer(range_reader &t)
{
    return byte_range_buffer(t);
}

inline sequence_buffer<std::string> create_buffer(std::string &t)
{
    return sequence_buffer<std::string>(t);
}

inline sequence_buffer<std::vector<unsigned char>> create_buffer(std::vector<unsigned char> &t)
{
    return sequence_buffer<std::vector<unsigned char>>(t);
}

inline base_buffer dummy_buffer()
{
    return base_buffer();
}


class unpacker
{
    range_reader m_range;
public:

    unpacker(const unsigned char *begin, const unsigned char *end)
        : m_range(refrange::immutable_range(begin, end))
    {
    }

    const range_reader& range()const{ return m_range; }

    unpacker& drop()
    {
        return unpack(base_buffer());
    }

    template<class BUFFER>
        unpacker& unpack(BUFFER &b)
        {
            auto p_head_byte=m_range.current();
			switch(m_range.read_byte())
            {
                case nil_tag::bits:
                    b.read_from(nil_tag(p_head_byte), m_range);
                    break;

                case false_tag::bits:
                    b.read_from(false_tag(p_head_byte), m_range);
                    break;

                case true_tag::bits:
                    b.read_from(true_tag(p_head_byte), m_range);
                    break;

                case float32_tag::bits:
                    b.read_from(float32_tag(p_head_byte), m_range);
                    break;

                case float64_tag::bits:
                    b.read_from(float64_tag(p_head_byte), m_range);
                    break;

                case uint8_tag::bits:
                    b.read_from(uint8_tag(p_head_byte), m_range);
                    break;

                case uint16_tag::bits:
                    b.read_from(uint16_tag(p_head_byte), m_range);
                    break;

                case uint32_tag::bits:
                    b.read_from(uint32_tag(p_head_byte), m_range);
                    break;

                case uint64_tag::bits:
                    b.read_from(uint64_tag(p_head_byte), m_range);
                    break;

                case int8_tag::bits:
                    b.read_from(int8_tag(p_head_byte), m_range);
                    break;

                case int16_tag::bits:
                    b.read_from(int16_tag(p_head_byte), m_range);
                    break;

                case int32_tag::bits:
                    b.read_from(int32_tag(p_head_byte), m_range);
                    break;

                case int64_tag::bits:
                    b.read_from(int64_tag(p_head_byte), m_range);
                    break;

                    // sequence
                case bin8_tag::bits:
                    m_range.skip(1);
                    b.read_from(bin8_tag(p_head_byte), m_range);
                    break;

                case bin16_tag::bits:
                    m_range.skip(2);
                    b.read_from(bin16_tag(p_head_byte), m_range);
                    break;

                case bin32_tag::bits:
                    m_range.skip(4);
                    b.read_from(bin32_tag(p_head_byte), m_range);
                    break;

                case str8_tag::bits:
                    m_range.skip(1);
                    b.read_from(str8_tag(p_head_byte), m_range);
                    break;

                case str16_tag::bits:
                    m_range.skip(2);
                    b.read_from(str16_tag(p_head_byte), m_range);
                    break;

                case str32_tag::bits:
                    m_range.skip(4);
                    b.read_from(str32_tag(p_head_byte), m_range);
                    break;

                    // collection
                case array16_tag::bits:
                    m_range.skip(2);
                    b.read_from(array16_tag(p_head_byte), m_range);
                    break;

                case array32_tag::bits:
                    m_range.skip(4);
                    b.read_from(array32_tag(p_head_byte), m_range);
                    break;

                case map16_tag::bits:
                    m_range.skip(2);
                    b.read_from(map16_tag(p_head_byte), m_range);
                    break;

                case map32_tag::bits:
                    m_range.skip(4);
                    b.read_from(map32_tag(p_head_byte), m_range);
                    break;

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
                    if(positive_fixint_tag::is_match(*p_head_byte)){
                        // fixint
                        b.read_from(positive_fixint_tag(p_head_byte), m_range);
                    }
                    else if(negative_fixint_tag::is_match(*p_head_byte)){
                        // fixint
                        b.read_from(negative_fixint_tag(p_head_byte), m_range);
                    }
                    else if(fixstr_tag::is_match(*p_head_byte)){
                        // fixstr
                        b.read_from(fixstr_tag(p_head_byte), m_range);
                    }
                    else if(fixarray_tag::is_match(*p_head_byte)){
                        // collection
                        b.read_from(fixarray_tag(p_head_byte), m_range);
                    }
                    else if(fixmap_tag::is_match(*p_head_byte)){
                        // collection
                        b.read_from(fixmap_tag(p_head_byte), m_range);
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
        auto head_byte=m_range.peek_byte();
        return head_byte==nil_tag::bits; 
    }

    bool is_bool()
    {
        auto head_byte=m_range.peek_byte();
        return head_byte==false_tag::bits
            || head_byte==true_tag::bits;
    }

    bool is_sigend()
    {
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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
        auto head_byte=m_range.peek_byte();
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

};


} // namespace
} // namespace

