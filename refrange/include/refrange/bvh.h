#pragma once
#include "reader.h"
#include <memory>


namespace refrange {
namespace bvh {


class node
{
};


class loader
{
    std::shared_ptr<node> m_root;

public:
    bool load(const immutable_range &r)
    {
        line_reader reader(r);
        if(!parse_hierarchy(reader)){
            return false;
        }
        if(!parse_frames(reader)){
            return false;
        }
        return true;
    }

private:
    bool parse_hierarchy_node(line_reader &reader)
    {
		auto line = reader.get_line();
        auto tokens=line.split();
        if (tokens.empty()){
            return false;
        }

		auto key = tokens.front();
        if(key=="ROOT"){
            assert(!m_root);
            if(reader.get_line().trim()!="{"){
                return false;
            }
            auto offsets=reader.get_line().split();
            auto channels=reader.get_line().split();

            // nest
			if (!parse_hierarchy_node(reader)){
				return false;
			}

			return reader.get_line().trim() == "}";
        }
        else if(key=="End"){
            if(reader.get_line().trim()!="{"){
                return false;
            }
            auto offsets=reader.get_line().split();
            return reader.get_line().trim()=="}";
        }
        else{
            // unknown
            return false;
        }
    }

    bool parse_hierarchy(line_reader &reader)
    {
        auto line=reader.get_line();
        if(line!="HIERARCHY"){
            return false;
        }
        if(!parse_hierarchy_node(reader)){
            return false;
        }
        return true;
    }

    bool parse_frames(line_reader &reader)
    {
        if(reader.get_line()!="MOTION"){
            return false;
        }
        size_t frames=0;
        {
            auto line=reader.get_line();
            if(!line.startswith("Frames:")){
                return false;
            }
            frames=(size_t)atoi((char*)&line[7]);
        }
        {
            auto line=reader.get_line();
            if(!line.startswith("Frame Time:")){
                return false;
            }
        }
        for(size_t i=0; i<frames; ++i){
            auto line=reader.get_line();
        }
        return true;
    }
};

} // namespace
} // namespace
