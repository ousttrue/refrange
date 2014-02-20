#pragma once

namespace refrange {

struct immutable_range
{
    const unsigned char *begin;
    const unsigned char *end;

public:
	immutable_range()
        : begin(0), end(0)
    {} 

	immutable_range(const unsigned char *b, const unsigned char *e)
        : begin(b), end(e)
    {}
};


class range_reader
{
    immutable_range m_range;
    const unsigned char *m_current;

public:
    range_reader(const immutable_range &range)
        : m_range(range), m_current(m_range.begin)
    {}

    immutable_range &get_range(){ return m_range; }

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
        return m_range.end-m_current;
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
        return len;
    }
};


struct mutable_range
{
    unsigned char *begin;
    unsigned char *end;

public:
	mutable_range()
        : begin(0), end(0)
    {} 

	mutable_range(unsigned char *b, unsigned char *e)
        : begin(b), end(e)
    {}
};


class range_writer
{
    mutable_range m_range;
    unsigned char *m_current;

public:
    range_writer(const mutable_range &range)
        : m_range(range), m_current(m_range.begin)
    {}

    mutable_range &get_range(){ return m_range; }

    size_t size()const{ return m_current-m_range.begin; }

    size_t write(const unsigned char *p, size_t len)
    {
        if(!p){
            return 0;
        }
        if(len==0){
            return 0;
        }
        if(m_current+len>m_range.end){
            throw std::range_error(__FUNCTION__);
        }

        std::copy(p, p+len, m_current);
        m_current+=len;
        return len;
    }
};


} // namespace

