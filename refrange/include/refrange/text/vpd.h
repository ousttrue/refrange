#pragma once
#include "text_reader.h"


namespace refrange {
namespace text {
namespace vpd {


struct vec3
{
    float x;
    float y;
    float z;
};


struct quaternaion
{
    float x;
    float y;
    float z;
    float w;
};


struct bonetransformation
{
    std::string name;
    vec3 translation;
    quaternaion rotation;
};


class loader
{
    std::vector<bonetransformation> m_bones;

public:
    std::vector<bonetransformation> &get_bones(){ return m_bones; }

    bool load(const immutable_range &r)
    {
        line_reader reader(r);

        if(reader.get_line()!="Vocaloid Pose Data file"){
            return false;
		}

        reader.get_line(); //miku.osm;

        // bones
		int bonenum = reader.get_line().to_int();

        for(int i=0; i<bonenum; ++i){
            m_bones.push_back(bonetransformation());
            auto &bone=m_bones.back();

            // name
            {
                auto line=reader.get_line();
                auto splited=line.split('{');
                auto it=splited.begin();
                ++it;
                bone.name=it->to_str();
            }

            // translation
            {
                auto line=reader.get_line();
                auto splited=line.split();
                auto it=splited.begin();
                bone.translation.x=it->to_float();
                ++it;
                bone.translation.y=it->to_float();
                ++it;
                bone.translation.z=it->to_float();
            }

            // rotation
            {
                auto line=reader.get_line();
                auto splited=line.split();
                auto it=splited.begin();
                bone.rotation.x=it->to_float();
                ++it;
                bone.rotation.y=it->to_float();
                ++it;
                bone.rotation.z=it->to_float();
                ++it;
                bone.rotation.w=it->to_float();
            }

            {
                auto line=reader.get_line();
                assert(line=="}");
            }
        }

        return true;
    }
};

}  // namespace
}  // namespace
}  // namespace

