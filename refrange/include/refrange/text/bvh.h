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
    bool parse_joint(line_reader &reader, hierarchy *parent)
    {
        {
            auto offsets_line=reader.get_line().trim();
            auto splited=offsets_line.split();
            auto it=splited.begin();
            assert(it->to_str()=="OFFSET");
            ++it;
            parent->value.offset.x=it->to_int();
            ++it;
            parent->value.offset.y=it->to_int();
            ++it;
            parent->value.offset.z=it->to_int();
        }

        auto channels_line=reader.get_line().trim();

        while(true)
        {
            auto root_line=reader.get_line().trim();
            if(root_line=="}"){
                // close
                break;
            }

            auto splited=root_line.split();
            auto it=splited.begin();
            auto key=it->to_str();
            if(key=="JOINT"){
                parent->children.push_back(hierarchy());
                ++it;
                parent->children.back().value.name=it->to_str();

                auto open_line=reader.get_line().trim();
                assert(open_line=="{");

				parse_joint(reader, &parent->children.back());
            }
            else if(key=="End"){
                parent->children.push_back(hierarchy());

                auto open_line=reader.get_line().trim();
                assert(open_line=="{");

                {
                    auto offsets_line=reader.get_line().trim();
                    auto splited=offsets_line.split();
                    auto it=splited.begin();
                    assert(it->to_str()=="OFFSET");
                    ++it;
                    parent->children.back().value.offset.x=it->to_int();
                    ++it;
                    parent->children.back().value.offset.y=it->to_int();
                    ++it;
                    parent->children.back().value.offset.z=it->to_int();
                }

                auto close_line=reader.get_line().trim();
                assert(close_line=="}");
            }
            else{
                assert(false);
                return false;
            }
        }

        return true;
    }

    bool parse_hierarchy(line_reader &reader)
    {
        auto line=reader.get_line();
        if(line!="HIERARCHY"){
            return false;
        }
        auto root_line=reader.get_line().trim().split();
        m_hierarchy.value.name=root_line.begin()->to_str();

        auto open_line=reader.get_line().trim();
        assert(open_line=="{");
        if(!parse_joint(reader, &m_hierarchy)){
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
			auto splited = line.split(':');
			auto it = splited.begin();
            if(*it!="Frames"){
                return false;
            }
			++it;
            frames=it->trim().to_int();
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
