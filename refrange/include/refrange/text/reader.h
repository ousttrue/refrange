#pragma once
#include "../reader.h"


namespace refrange {
namespace text {


class text_reader: public range_reader
{
public:
    text_reader(const immutable_range &r)
        : range_reader(r)
    {}

    int get_int()
    {
        auto range=get_range().find_range_if(&is_int<type>, get_current());
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
		if (get_current() < get_range().end()){
			increment();
		}
        if(end!=get_range().begin()){
            if(end[-1]=='\r'){
                // CR
                --end;
            }
        }
        return immutable_range(begin, end);
    }
};

} // namespace
} // namespace
