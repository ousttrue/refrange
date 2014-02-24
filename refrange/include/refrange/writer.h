#pragma once
#include "range.h"


namespace refrange {

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

    size_t write_str(const std::string &str, size_t len=0)
    {
        if(len==0){
            len=str.size();
        }
        auto write_size=std::min(str.size(), len);
        auto size=write((const unsigned char*)str.c_str(), write_size);

        // fill zero
        for(auto i=size; i<len; ++i, ++size){
            unsigned char zero=0;
            write_value(zero);
        }

		return size;
    }

    template<typename T>
        size_t write_value(const T &t)
        {
            auto size=write((const unsigned char*)&t, sizeof(T));
            assert(size==sizeof(T));
            return size;
        }

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

		//std::copy(p, p + len, m_current);
		memcpy(m_current, p, len);

        m_current+=len;
        return len;
    }
};

}
