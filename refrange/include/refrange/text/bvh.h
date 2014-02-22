#pragma once
#include "reader.h"
#include "../tree.h"
#include <memory>


namespace refrange {
namespace text {
namespace bvh {


struct vec3
{
    float x;
    float y;
    float z;

    bool operator==(const vec3 &rhs)const
    {
        return x==rhs.x && y==rhs.y && z==rhs.z;
    }
};


struct joint
{
    std::string name;
    vec3 offset;

    bool operator==(const joint &rhs)const
    {
        return name==rhs.name && offset==rhs.offset;
    }
};

typedef node<joint> hierarchy;

class loader
{
    hierarchy m_hierarchy;

public:
    hierarchy &get_hierarchy(){ return m_hierarchy; }

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
    bool parse_hierarchy_node(line_reader &reader, hierarchy &parent)
    {
		auto line = reader.get_line();
        auto tokens=line.split();
        if (tokens.empty()){
            return false;
        }

        auto it=tokens.begin();
        if(it==tokens.end()){
            return false;
        }
		auto key = *it;
		++it;
        if(key=="ROOT"){
            // root
            auto &root=m_hierarchy.value;
            root.name=it->to_str();

            if(reader.get_line().trim()!="{"){
                return false;
            }

			auto offsets = reader.get_line().split();
			{
				auto it = offsets.begin();
				if (*it != "OFFSET"){
					return false;
				}
				++it;
				root.offset.x = it->to_int();
				++it;
				root.offset.y = it->to_int();
				++it;
				root.offset.z = it->to_int();
			}

            auto channels=reader.get_line().split();

            // nest
			if (!parse_hierarchy_node(reader, m_hierarchy)){
				return false;
			}

			return reader.get_line().trim() == "}";
        }
		else if (key == "End"){
			if (reader.get_line().trim() != "{"){
				return false;
			}

			joint end;
			end.name = it->to_str();

			auto offsets = reader.get_line().split();
			{
				auto it = offsets.begin();
				if (*it != "OFFSET"){
					return false;
				}
				++it;
				end.offset.x = it->to_int();
				++it;
				end.offset.y = it->to_int();
				++it;
				end.offset.z = it->to_int();
			}

			parent.children.push_back({ end });

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
        if(!parse_hierarchy_node(reader, m_hierarchy)){
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
} // namespace
