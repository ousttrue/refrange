#pragma once
#include "range.h"


namespace refrange {

class range_reader
{
    immutable_range m_range;
    const unsigned char *m_current;

public:
    range_reader(const immutable_range &range)
        : m_range(range), m_current(m_range.begin())
    {}

    immutable_range &get_range(){ return m_range; }

    const unsigned char *current()const
    { 
        return m_current; 
    }

    bool is_end()const{ return m_current>=m_range.end(); }

    unsigned char peek_byte()
    {
        return *m_current;
    }

    unsigned char read_byte()
    {
        unsigned char c;
        read_value<unsigned char>(&c);
        return c;
    }

    void skip(size_t s)
    {
        if(m_current+s>m_range.end()){
            throw std::range_error(__FUNCTION__);
        }
        m_current+=s;
    } 

    template<typename T, typename W>
        void read_value(W *w)
        {
            T n;
            size_t size=read((unsigned char*)&n, sizeof(T));
            *w=n;
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

        std::copy(m_current, m_current+len, p);
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


class line_reader: public range_reader
{
public:
    line_reader(const immutable_range &r)
        : range_reader(r)
    {}

    immutable_range get_line()
    {
        auto begin=current();
        auto end=find('\n');
        increment();
        return immutable_range(begin, end);
    }
};


} // namespace
