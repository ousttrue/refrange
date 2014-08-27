#pragma once
#include "range.h"
#ifdef _MSC_VER
#else
#include <string.h>
#endif

namespace refrange {

class range_reader
{
    immutable_range m_range;

    const unsigned char *m_current;

public:
    typedef immutable_range::type type;

    range_reader(const immutable_range &range)
        : m_range(range), m_current(m_range.begin())
    {}

    immutable_range &get_range(){ return m_range; }

    const unsigned char *get_current()const
    { 
        return m_current; 
    }
protected:
	void set_current(const unsigned char *p){ m_current = p;  }
public:

    bool is_end()const{ return m_current>=m_range.end(); }

    unsigned char peek_byte()
    {
        return *m_current;
    }

    unsigned char read_byte()
    {
        unsigned char c;
        read_value(c);
        return c;
    }

    immutable_range read_range(size_t bytes)
    {
        if(m_current+bytes>m_range.end()){
            throw std::range_error(__FUNCTION__);
        }
        immutable_range r(m_current, m_current+bytes);
        m_current+=bytes;
        return r;
    } 

    void skip(size_t bytes)
    {
        read_range(bytes);
    }

    std::string read_str(size_t bytes)
    {
        auto r=read_range(bytes);
        return r.to_str();
    }

    template<typename T>
        void read_value(T &t)
        {
			size_t size = read((unsigned char*)&t, sizeof(T));
        }

    size_t remain_size()
    {
        if(m_current==0){
            return 0;
        }
        return m_range.end()-m_current;
    }

    size_t read(unsigned char *p, size_t len)
    {
        if(p==0){
            return 0;
        }
        if(len==0){
            return 0;
        }
        if(m_current+len>m_range.end()){
            throw std::range_error(__FUNCTION__);
        }

        //std::copy(m_current, m_current+len, p);
		memcpy(p, m_current, len);

        m_current+=len;
        return len;
    }

    const unsigned char* find(unsigned char c)
    {    
		auto e = m_range.end();
        for(; m_current!=e; ++m_current){
            if(*m_current==c){
                break;
            }
        }
        return m_current;
    }

    void increment()
    {
        if(m_current+1>m_range.end()){
            throw std::range_error(__FUNCTION__);
        }
        ++m_current;
    }
};


} // namespace
