#pragma once

namespace refrange {

struct range
{
    const unsigned char *begin;
    const unsigned char *end;

public:
    byte_range()
        : begin(0), end(0)
    {} 

    byte_range(const unsigned char *begin, const unsigned char *end)
        : m_begin(begin), m_end(end)
    {}

    const unsigned char *begin()const{ return m_begin; }
    const unsigned char *end()const{ return m_end; }
};


class range_reader
{
    range &m_range;
    const unsigned char *m_current;

public:
    range_reader(const range &range)
        : m_range(range), m_current(m_range.begin)
    {}

    const unsigned char *current()const
    { 
        return m_current; 
    }

    bool is_end()const{ return m_current>=m_range.end; }

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
        if(m_current+s>m_range.end){
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
        return m_reange.end-m_current;
    }

    size_t read(unsigned char *p, size_t len)
    {
        if(p==0){
            return 0;
        }
        if(len==0){
            return 0;
        }
        if(m_current+len>m_range.end){
            throw std::range_error(__FUNCTION__);
        }

        std::copy(m_current, m_current+len, p);
        m_current+=len;
        return copysize;
    }
};

} // namespace

