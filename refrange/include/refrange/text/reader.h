#pragma once
#include "../reader.h"


namespace refrange {
namespace text {

inline bool is_int(const unsigned char *p)
{
    switch(*p)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
    }
    return false;
}


class text_reader: public range_reader
{
public:
    text_reader(const immutable_range &r)
        : range_reader(r)
    {}

    int get_int()
    {
        auto range=get_range().find_range_if(&is_int, get_current());
        if(!range){
            throw std::exception(__FUNCTION__);
        }
		set_current(range.end());

        return range.to_int();
    }
};


class line_reader: public text_reader
{
public:
    line_reader(const immutable_range &r)
        : text_reader(r)
    {}

    immutable_range get_line()
    {
        auto begin=get_current();
        auto end=find('\n');
        increment();
        return immutable_range(begin, end);
    }
};

} // namespace
} // namespace
