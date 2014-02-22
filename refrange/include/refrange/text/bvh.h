#pragma once
#include "reader.h"
#include "../tree.h"
#include <memory>
#include <array>


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


enum channel_t
{
    channel_None,
    channel_Xposition,
    channel_Yposition,
    channel_Zposition,
    channel_Xrotation,
    channel_Yrotation,
    channel_Zrotation,
};


struct joint
{
    std::string name;
    vec3 offset;

    std::array<channel_t, 6> channels;
    unsigned char channel_size;

    joint()
        : channel_size(0)
    {
    }

    joint(const std::string &_name, const vec3 _o)
        : name(_name), offset(_o), channel_size(0)
    {}

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

    void assign_offset(joint &j, const immutable_range &r)
    {
        auto splited=r.split();
        auto it=splited.begin();
        assert(it->to_str()=="OFFSET");
        ++it;
        j.offset.x=it->to_int();
        ++it;
        j.offset.y=it->to_int();
        ++it;
        j.offset.z=it->to_int();
    }

    bool parse_joint(line_reader &reader, hierarchy *parent)
    {
        auto offsets_line=reader.get_line().trim();
        assign_offset(parent->value, offsets_line);

        auto channels_line=reader.get_line().trim();
        //assign_channel(parent->value, channels_line);

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

                auto offsets_line=reader.get_line().trim();
                assign_offset(parent->children.back().value, offsets_line);

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
