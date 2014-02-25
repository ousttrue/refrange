#pragma once
#include <stdexcept>
#include <list>
#include <functional>
#include <fstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "text.h"


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
	typedef std::function<bool(T)> pred;

public:
    typedef T type;

    range()
        : m_begin(0), m_end(0)
    {}

    range(T begin, T end)
        : m_begin(begin), m_end(end)
    {
    }

    T begin()const{ return m_begin; }
    T end()const{ return m_end; }
    size_t size()const{ return m_end-m_begin; }

    operator bool()const{ return m_begin<m_end; }

    typename value_from_pointer<T>::type &operator[](size_t index){ return m_begin[index]; }

    std::string to_str()const{ 
        auto end=m_begin;
        for(; end!=m_end; ++end){
            if(*end=='0'){
                break;
            }
        }
        return std::string(m_begin, end); 
    }

    int to_int()const
    {
        bool is_negative=false;
        int n = 0;
        for (auto p = m_begin; p != m_end; ++p)
        { 
            if(text::is_digit(p)){
                n = n * 10 + (*p-'0');
            }
            else{
                if(n==0){
                    if(*p=='-'){
                        is_negative=true;
                    }
                    else{
						break;
                    }
                }
				else{
					break;
				}
            }
        }
		return is_negative ? -n : n;
    }

    double to_double()const
    {
        return atof((const char*)m_begin);
    }

    float to_float()const
    {
        return static_cast<float>(to_double());
    }

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

    T find(T p)const
    {
        for(auto it=m_begin; it!=m_end; ++it){
            bool is_match=true;
            auto k=p;
            for(auto j=it; j!=m_end && *p!='\0'; ++j, ++k){
                if(*j!=*k){
                    is_match=false;
                    break;
                }
            }
            if(is_match){
                return it;
            }
        }
        return m_end;
    }

	range<T> find_range_if(const pred &func)const
	{
		return find_range_if(func, m_begin);
	}

	range<T> find_range_if(const pred &func, T p)const
	{
		T begin = p;
        // find match
        for(; begin!=m_end; ++begin){
            if(func(begin)){
                break;
            }
        }
        if(begin==m_end){
            return range<T>(m_end, m_end);
        }
        // find not match
        auto end=begin+1;
        for(; end!=m_end; ++end){
            if(!func(end)){
                break;
            }
        }
        return immutable_range(begin, end);
    }

    std::list<range<T>> split(char c)const
    {
        auto delimiter=[c](T t)
        {
            return *t==c;
        };

        return split(delimiter);
    }

    std::list<range<T>> split()const
    {
        return split(&text::is_space<T>);
    }

    std::list<range<T>> split(const pred &func)const
    {
        std::list<range<T>> splited;
        if(m_begin==m_end){
            return splited;
        }

        auto begin=m_begin;
        while(begin!=m_end){
			auto range = find_range_if(std::not1(func), begin);
            if(!range){
                break;
            }
            splited.push_back(range);
            begin=range.end();
        }
        return splited;
    }

    range<T> ltrim()const
    {
        auto range=find_range_if(&text::is_space<T>);
        auto begin=m_begin;
        if(range && range.begin()==m_begin){
            begin=range.end();
        }
        return immutable_range(begin, m_end);
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


inline immutable_range emptyrange()
{
    return immutable_range(
            (const unsigned char*)0,
            (const unsigned char*)0);
}


inline immutable_range vectorrange(const std::vector<unsigned char> &v)
{
    if(v.empty()){
        return emptyrange();
    }

    return immutable_range(&v[0], &v[0]+v.size());
}


inline std::vector<unsigned char> readfile(const char *path)
{
    std::vector<unsigned char> buf;
    std::ifstream ifs(path, std::ios::binary);
    if(!ifs){
        return buf;
    }

	ifs.seekg (0, std::ios::end);
	buf.resize((size_t)ifs.tellg());
    if(buf.empty()){
        return buf;
    }
    ifs.seekg (0, std::ios::beg);
	ifs.read ((char*)&buf[0], buf.size());

    return std::move(buf);
}


} // namespace
