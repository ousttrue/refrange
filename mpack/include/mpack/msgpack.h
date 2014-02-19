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

    //////////////////////////////////////////////////////////////////////////////
    // message
    //////////////////////////////////////////////////////////////////////////////

    struct nil_tag
    {
        enum bits_t { bits=0xc0 };
    };

    struct false_tag
    {
        enum bits_t { bits=0xc2 };

        typedef bool fixed_value_type;
        fixed_value_type value(){ return false; };
    };

    struct true_tag
    {
        enum bits_t { bits=0xc3 };

        typedef bool fixed_value_type;
        fixed_value_type value(){ return true; };
    };

    struct bin8_tag
    {
        enum bits_t { bits=0xc4 };
    };

    struct bin16_tag
    {
        enum bits_t { bits=0xc5 };
    };

    struct bin32_tag
    {
        enum bits_t { bits=0xc6 };
    };

    struct ext8_tag
    {
        enum bits_t { bits=0xc7 };
    };

    struct ext16_tag
    {
        enum bits_t { bits=0xc8 };
    };

    struct ext32_tag
    {
        enum bits_t { bits=0xc9 };
    };

    struct float32_tag
    {
        enum bits_t { bits=0xca };
        typedef float read_type;
    };

    struct float64_tag
    {
        enum bits_t { bits=0xcb };
        typedef double read_type;
    };

    struct uint8_tag
    {
        enum bits_t { bits=0xcc };
        typedef unsigned char read_type;
    };

    struct uint16_tag
    {
        enum bits_t { bits=0xcd };
        typedef unsigned short read_type;
    };

    struct uint32_tag
    {
        enum bits_t { bits=0xce };
        typedef unsigned int read_type;
    };

    struct uint64_tag
    {
        enum bits_t { bits=0xcf };
        typedef unsigned long long read_type;
    };

    struct int8_tag
    {
        enum bits_t { bits=0xd0 };
        typedef char read_type;
    };

    struct int16_tag
    {
        enum bits_t { bits=0xd1 };
        typedef short read_type;
    };

    struct int32_tag
    {
        enum bits_t { bits=0xd2 };
        typedef int read_type;
    };

    struct int64_tag
    {
        enum bits_t { bits=0xd3 };
        typedef long long read_type;
    };

    struct fixext1_tag
    {
        enum bits_t { bits=0xd4 };
    };

    struct fixext2_tag
    {
        enum bits_t { bits=0xd5 };
    };

    struct fixext4_tag
    {
        enum bits_t { bits=0xd6 };
    };

    struct fixext8_tag
    {
        enum bits_t { bits=0xd7 };
    };

    struct fixext16_tag
    {
        enum bits_t { bits=0xd8 };
    };

    struct str8_tag
    {
        enum bits_t { bits=0xd9 };
    };

    struct str16_tag
    {
        enum bits_t { bits=0xda };
    };

    struct str32_tag
    {
        enum bits_t { bits=0xdb };
    };

    struct no_collection_tag{};
    struct array_tag{};
    struct map_tag{};

    struct array16_tag
    {
        enum is_array_t { is_array=1 };
        enum bits_t { bits=0xdc };

        unsigned short len;
        array16_tag(unsigned short l)
            : len(l)
        {}
    };

    struct array32_tag
    {
        enum is_array_t { is_array=1 };
        enum bits_t { bits=0xdd };

        unsigned int len;
        array32_tag(unsigned int l)
            : len(l)
        {}
    };

    struct map16_tag
    {
        enum is_map_t { is_map=1 };
        enum bits_t { bits=0xde };

        unsigned short len;
        map16_tag(unsigned short l)
            : len(l)
        {}
    };

    struct map32_tag
    {
        enum is_map_t { is_map=1 };
        enum bits_t { bits=0xdf };

        unsigned int len;
        map32_tag(unsigned int l)
            : len(l)
        {}
    };

	template<class T>
	bool partial_bit_equal(unsigned char byte)
	{
		return T::bits == (T::mask & byte);
	}

	template<class T>
	char extract_head_byte(unsigned char byte)
	{
		return static_cast<char>(byte & ~T::mask);
	}

    struct positive_fixint_tag
    {
        enum bits_t { bits=0x00 };
        enum mask_t { mask=0x80 };

        unsigned char head_byte;
        positive_fixint_tag(unsigned char b)
            : head_byte(b)
        {}

        typedef char fixed_value_type;
        fixed_value_type value(){ return static_cast<fixed_value_type>(head_byte); };
    };
    struct fixmap_tag
    {
        enum is_map_t { is_map=1 };
        enum bits_t { bits=0x80 };
        enum mask_t { mask=0xF0 };

        unsigned char len;
        fixmap_tag(unsigned char l)
            : len(l)
        {}
    };
    struct fixarray_tag
    {
        enum is_array_t { is_array=1 };
        enum bits_t { bits=0x90 };
        enum mask_t { mask=0xf0 };

        unsigned char len;
        fixarray_tag(unsigned char l)
            : len(l)
        {}
    };
    struct fixstr_tag
    {
        enum bits_t { bits=0xa0 };
        enum mask_t { mask=0xe0};

        unsigned char head_byte;
        fixstr_tag(unsigned char b)
            : head_byte(b)
        {}
    };
    struct negative_fixint_tag
    {
        enum bits_t { bits=0xe0 };
        enum mask_t { mask=0xe0 };

        unsigned char head_byte;
        negative_fixint_tag(unsigned char b)
            : head_byte(b)
        {}

        typedef char fixed_value_type;
        fixed_value_type value(){ return -static_cast<fixed_value_type>(extract_head_byte<negative_fixint_tag>(head_byte)); };
    };


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

        template<class Tag>
		void _read_from(Tag &tag, reader_t &reader, unsigned int count, no_collection_tag)
        {
            // not reach here
            assert(false);
        }

        template<class Tag>
            void _read_from(Tag &tag, reader_t &reader, unsigned int count, map_tag)
            {
                type=collection_map;
                size=tag.len;
            }

        template<class Tag>
            void _read_from(Tag &tag, reader_t &reader, unsigned int count, array_tag)
            {
                type=collection_array;
                size=tag.len;
            }

        template<class Tag>
            void read_from(Tag &tag, reader_t &reader, unsigned int count=0)
            {
                _read_from(tag, reader, count, collection_traits<Tag>::tag());
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
    // read_assign
	struct no_value_tag{};
    struct read_value_tag{};
    struct fixed_value_tag{};
    template<class Tag
        , class TagEnable=void
        >
        struct read_traits
        {
            typedef no_value_tag tag;
        };
    template<class Tag>
        struct read_traits<Tag
        , typename std::enable_if<std::is_arithmetic<typename Tag::read_type>::value>::type
        >
        {
            typedef read_value_tag tag;
        };
    template<class Tag>
        struct read_traits<Tag
        , typename std::enable_if<std::is_arithmetic<typename Tag::fixed_value_type>::value>::type
        >
        {
            typedef fixed_value_tag tag;
        };

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
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, no_value_tag, arithmetic_copy_tag)
        {
            assert(count==0);
            // do nothing
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, read_value_tag, arithmetic_copy_tag)
        {
            assert(count==0);

            Tag::read_type t;
            auto size = reader((unsigned char*)&t, sizeof(t));
            assert(size == sizeof(t));

            *v = static_cast<Value>(t);
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, fixed_value_tag, arithmetic_copy_tag)
        {
            assert(count==0);
            *v = static_cast<Value>(tag.value());
        }

    //
    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, no_value_tag, nocopy_tag)
        {
            assert(count==0);
            // do nothing
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, read_value_tag, nocopy_tag)
        {
            assert(count==0);

            Tag::read_type t;
            auto size = reader((unsigned char*)&t, sizeof(t));
            assert(size == sizeof(t));

            // read only
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, fixed_value_tag, nocopy_tag)
        {
            assert(count==0);
            // do nothing
        }

    //
    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, no_value_tag, sequence_tag)
        {
			if (count == 0){
				return;
			}
            v->resize(count);
            auto size=reader((unsigned char*)&((*v)[0]), count);
            assert(size==count);
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, read_value_tag, sequence_tag)
        {
            assert(false);
            // not reach here
        }

    template<class Tag, class Value>
        void read_and_assign(Tag &tag, reader_t &reader, unsigned int count, Value *v, fixed_value_tag, sequence_tag)
        {
            assert(false);
            // not reach here
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
        void read_from(Tag &tag, reader_t &reader, unsigned int count=0)
        {
            read_and_assign(tag, reader, count,
                    m_p, 
                    read_traits<Tag>::tag(),
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

        unsigned char read_head_byte()
        {
			unsigned char c;
			read_value<unsigned char>(&c);
            return c;
        }

        template<class BUFFER>
        unpacker& unpack(BUFFER &b)
        {
            auto head_byte=read_head_byte();
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
                        b.read_from(bin8_tag(), m_reader, len);
                    }
                    break;

				case bin16_tag::bits:
                    {
						unsigned short len;
                        read_value<unsigned short>(&len);
                        b.read_from(bin16_tag(), m_reader, len);
                    }
                    break;

				case bin32_tag::bits:
                    {
						unsigned int len;
                        read_value<unsigned int>(&len);
                        b.read_from(bin32_tag(), m_reader, len);
                    }
                    break;

				case str8_tag::bits:
                    {
                        unsigned char len;
                        read_value<unsigned char>(&len);
                        b.read_from(str8_tag(), m_reader, len);
                    }
                    break;

				case str16_tag::bits:
                    {
                        unsigned short len;
                        read_value<unsigned short>(&len);
                        b.read_from(str16_tag(), m_reader, len);
                    }
                    break;

				case str32_tag::bits:
                    {
                        unsigned int len;
                        read_value<unsigned int>(&len);
                        b.read_from(str32_tag() ,m_reader, len);
                    }
                    break;

                    // collection
				case array16_tag::bits:
					{
                        unsigned short len;
                        read_value<unsigned short>(&len);
                        b.read_from(array16_tag(len), m_reader);
						/*
                        for(unsigned short i=0; i<len; ++i){
                            unpack(b);
                        }
						*/
                    }
                    break;

				case array32_tag::bits:
					{
						unsigned int len;
						read_value<unsigned int>(&len);
                        b.read_from(array32_tag(len), m_reader);
						/*
                        for(unsigned int i=0; i<len; ++i){
                            unpack(b);
                        }
						*/
					}
                    break;

				case map16_tag::bits:
					{
						unsigned short len;
						read_value<unsigned short>(&len);
                        b.read_from(map16_tag(len), m_reader);
					}
                    break;

				case map32_tag::bits:
					{
						unsigned int len;
						read_value<unsigned int>(&len);
                        b.read_from(map32_tag(len), m_reader);
					}
                    break;

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

                default:
                    if(partial_bit_equal<positive_fixint_tag>(head_byte)){
                        // char
                        b.read_from(positive_fixint_tag(head_byte), m_reader);
                    }
                    else if(partial_bit_equal<negative_fixint_tag>(head_byte)){
                        // uchar
                        b.read_from(negative_fixint_tag(head_byte), m_reader);
                    }
                    else if(partial_bit_equal<fixstr_tag>(head_byte)){
                        // str todo
                        auto len=extract_head_byte<fixstr_tag>(head_byte);
                        b.read_from(fixstr_tag(head_byte), m_reader, len);
                    }
                    else if(partial_bit_equal<fixarray_tag>(head_byte)){
                        // collection
                        unsigned char len=extract_head_byte<fixarray_tag>(head_byte);
                        b.read_from(fixarray_tag(len), m_reader);
                    }
                    else if(partial_bit_equal<fixmap_tag>(head_byte)){
                        // collection
                        unsigned char len=extract_head_byte<fixmap_tag>(head_byte);
                        b.read_from(fixmap_tag(len), m_reader);
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

        bool is_array()
        {
            auto head_byte=peek_byte();
            switch(head_byte)
            {
                case array16_tag::bits:
                case array32_tag::bits:
                    return true;
            }
            return partial_bit_equal<fixarray_tag>(head_byte);
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
            return partial_bit_equal<fixmap_tag>(head_byte);
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
            return partial_bit_equal<positive_fixint_tag>(head_byte)
                || partial_bit_equal<negative_fixint_tag>(head_byte);
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
    inline unpacker& operator>>(unpacker &unpacker, bool &t) { return unpacker.unpack(create_buffer(t)); }

    // signed
    inline unpacker& operator>>(unpacker &unpacker, char &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, short &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, int &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, long long &t) { return unpacker.unpack(create_buffer(t)); }

    // unsigned
    inline unpacker& operator>>(unpacker &unpacker, unsigned char &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned short &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned int &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, unsigned long long &t) { return unpacker.unpack(create_buffer(t)); }

    // float
    inline unpacker& operator>>(unpacker &unpacker, float &t) { return unpacker.unpack(create_buffer(t)); }
    inline unpacker& operator>>(unpacker &unpacker, double &t) { return unpacker.unpack(create_buffer(t)); }

    // str
    inline unpacker& operator>>(unpacker &unpacker, std::string &t) { return unpacker.unpack(create_buffer(t)); }

    // bin
    inline unpacker& operator>>(unpacker &unpacker, std::vector<unsigned char> &t) { return unpacker.unpack(create_buffer(t)); }

    // collection
    inline unpacker& operator>>(unpacker &unpacker, collection_context &t){ return unpacker.unpack(t); }

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

