#pragma once
#include <stdexcept>
#include <list>
#include <functional>
#include <assert.h>


namespace refrange {


template<class T>
   struct value_from_pointer
   {
   };
template<class T>
   struct value_from_pointer<T*>
   {
       typedef T type;
   };



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

    operator bool()const{ return m_begin<m_end; }

    typename value_from_pointer<T>::type &operator[](size_t index){ return m_begin[index]; }

    bool operator==(const range<T> &s)const
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

    bool startswith(const std::string &s)const
    {
        auto l=m_begin;
        auto r=s.begin();
        for(; l!=m_end && r!=s.end(); ++l, ++r)
        {
            if(*l!=*r){
                return false;
            }
        }
        return r==s.end();
    }

	range<T> find_range_if(std::function<bool(T)> pred)
	{
		return find_range_if(pred, m_begin);
	}

	range<T> find_range_if(std::function<bool(T)> pred, T p)
	{
		T begin = p;
        // find match
        for(; begin!=m_end; ++begin){
            if(pred(begin)){
                break;
            }
        }
        if(begin==m_end){
            return range<T>(m_end, m_end);
        }
        // find not match
        auto end=begin+1;
        for(; end!=m_end; ++end){
            if(!pred(end)){
                break;
            }
        }
        return immutable_range(begin, end);
    }

    static bool is_space(T t)
    {
        return (*t == ' ');
    }

    std::list<range<T>> split(std::function<bool(T)> delimiter=&is_space)
    {
        std::list<range<T>> splited;
        if(m_begin==m_end){
            return splited;
        }

        auto begin=m_begin;
        while(begin!=m_end){
			auto range = find_range_if(std::not1(delimiter), begin);
            if(!range){
                break;
            }
            splited.push_back(range);
            begin=range.end();
        }
        return splited;
    }

    range<T> &ltrim(std::function<bool(T)> delimiter=&is_space)
    {
        auto range=find_range_if(is_space);
        if(range){
            m_begin=range.end();
        }

        return *this;
    }

    range<T> &trim(std::function<bool(T)> pred=&is_space)
    {
        ltrim();

        return *this;
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
