#pragma once
#include <stdexcept>


namespace refrange {


template<typename T>
class range
{
    T m_begin;
    T m_end;

public:
    range()
        : m_begin(0), m_end(0)
    {}

    range(T begin, T end)
        : m_begin(begin), m_end(end)
    {
    }

    T begin()const{ return m_begin; }
    T end()const{ return m_end; }

    bool operator==(const std::string &s)const
    {
        auto l=m_begin;
        auto r=s.begin();
        for(; l!=m_end && r!=s.end(); ++l, ++r)
        {
            if(*l!=*r){
                return false;
            }
        }
        return l==m_end && r==s.end();
    }

    bool operator!=(const std::string &s)const
    {
        return !(*this==s);
    }
};
typedef range<const unsigned char*> immutable_range;
typedef range<unsigned char*> mutable_range;


inline immutable_range strrange(const char *begin)
{
    auto end=begin;
    for(; *end!='\0'; ++end)
        ;

    return immutable_range(
            (const unsigned char *)begin, 
            (const unsigned char *)end
            );
}


class range_writer
{
    mutable_range m_range;
    unsigned char *m_current;

public:
    range_writer(const mutable_range &range)
        : m_range(range), m_current(m_range.begin())
    {}

    mutable_range &get_range(){ return m_range; }

    size_t size()const{ return m_current-m_range.begin(); }

    size_t write(const unsigned char *p, size_t len)
    {
        if(!p){
            return 0;
        }
        if(len==0){
            return 0;
        }
        if(m_current+len>m_range.end()){
            throw std::range_error(__FUNCTION__);
        }

        std::copy(p, p+len, m_current);
        m_current+=len;
        return len;
    }
};

} // namespace
