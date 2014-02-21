#pragma once
#include "reader.h"


namespace refrange {
namespace bvh {

class loader
{
public:
    bool load(const immutable_range &r)
    {
        line_reader reader(r);
        if(!parse_herarchy(reader)){
            return false;
        }
        if(!parse_frames(reader)){
            return false;
        }
        return true;
    }

private:
    bool parse_herarchy(line_reader &reader)
    {
        auto line=reader.get_line();
        if(line!="HIERARCHY"){
            return false;
        }
        return false;
    }

    bool parse_frames(line_reader &reader)
    {
        return false;
    }
};

} // namespace
} // namespace
