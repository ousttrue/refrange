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
        auto range=get_range().find_range_if(&is_digit<type>, get_current());
        if(!range){
            throw std::exception(__FUNCTION__);
        }
		set_current(range.end());

        return range.to_int();
    }
};


struct is_emptyline
{
    bool operator()(const immutable_range &line)const
    {
        return line.ltrim()=="";
    }
};


struct space_deleter
{
    immutable_range operator()(const immutable_range &line)const
    {
        if(line.begin()==line.end()){
            return immutable_range();
        }
		auto it = line.end() - 1;
        for(; it>=line.begin(); --it)
        {
            if(!is_space(it)){
				break;
            }
        }
		immutable_range result(line.begin(), it + 1);
		return result;
    }
};


struct comment_deleter
{
    immutable_range operator()(const immutable_range &line)const
    {
        auto p=line.find((const unsigned char*)"//");
        return immutable_range(line.begin(), p);
    }
};


template<typename SkipLine, typename ModifyLine>
class base_line_reader: public text_reader
{
    public:
        base_line_reader(const immutable_range &r)
            : text_reader(r)
        {}

        immutable_range get_line()
        {
            while(!is_end()){
                auto begin=get_current();
                auto end=find('\n');
                if (get_current() < get_range().end()){
                    increment();
                }
                immutable_range line(begin, end);
                if(SkipLine()(line)){
                    continue;
                }
                return ModifyLine()(line);
            }

            return immutable_range();
        }
};
typedef base_line_reader<is_emptyline, space_deleter> line_reader;


} // namespace
} // namespace
