#pragma once
#include "text_reader.h"
#include "../tree.h"
#include <memory>
#include <array>
#include <string>


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
	bool operator!=(const vec3 &rhs)const
	{
		return !(*this == rhs);
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
    std::vector<channel_t> channels;
    std::vector<vec3> ends;

    bool operator==(const joint &rhs)const
    {
		if (name != rhs.name){
			return false;
		}
		if (offset != rhs.offset){
			return false;
		}
		if (channels != rhs.channels){
			return false;
		}
		if (ends != rhs.ends){
			return false;
		}
		return true;
    }
};


struct frame
{
    std::vector<float> values;
};


typedef node<size_t> hierarchy;


class loader
{
    std::vector<joint> m_joints;
    hierarchy m_hierarchy;
    std::vector<frame> m_frames;

public:
    std::vector<joint> &get_joints(){ return m_joints; }
    hierarchy &get_hierarchy(){ return m_hierarchy; }
    std::vector<frame> &get_frames(){ return m_frames; }
    size_t get_channel_count()const{
        size_t channels=0;
        for(auto it=m_joints.begin(); it!=m_joints.end(); ++it){
            channels+=it->channels.size();
        }
        return channels;
    }

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
        j.offset.x=it->to_float();
        ++it;
		j.offset.y = it->to_float();
        ++it;
		j.offset.z = it->to_float();
    }

    void assign_endsite(joint &j, const immutable_range &r)
    {
		j.ends.push_back(vec3());
        auto &end=j.ends.back();

        auto splited=r.split();
        auto it=splited.begin();
        assert(it->to_str()=="OFFSET");
        ++it;
        end.x=it->to_float();
        ++it;
		end.y = it->to_float();
        ++it;
		end.z = it->to_float();
    }

    void assign_channel(joint &j, const immutable_range &r)
    {
        auto splited=r.split();
        auto it=splited.begin();
        assert(it->to_str()=="CHANNELS");
        ++it;
        auto channels=it->to_int();
        for(int i=0; i<channels; ++i){
            ++it;
            if(*it=="Xposition"){
                j.channels.push_back(channel_Xposition);
            }
            else if(*it=="Yposition"){
                j.channels.push_back(channel_Yposition);
            }
            else if(*it=="Zposition"){
                j.channels.push_back(channel_Zposition);
            }
            else if(*it=="Xrotation"){
                j.channels.push_back(channel_Xrotation);
            }
            else if(*it=="Yrotation"){
                j.channels.push_back(channel_Yrotation);
            }
            else if(*it=="Zrotation"){
                j.channels.push_back(channel_Zrotation);
            }
            else{
                throw std::exception(__FUNCTION__);
            }
        }
    }

    bool parse_joint(line_reader &reader, hierarchy *parent)
    {
        auto offsets_line=reader.get_line().ltrim();
        assign_offset(m_joints[parent->value], offsets_line);

        auto channels_line=reader.get_line().ltrim();
        assign_channel(m_joints[parent->value], channels_line);

        while(true)
        {
            auto root_line=reader.get_line().ltrim();
            if(root_line=="}"){
                // close
                break;
            }

            auto splited=root_line.split();
            auto it=splited.begin();
            auto key=it->to_str();
            if(key=="JOINT"){
				size_t index = m_joints.size();
				m_joints.push_back(joint());
				auto pJoint = &m_joints.back();

                ++it;
                pJoint->name=it->to_str();

                auto open_line=reader.get_line().ltrim();
                assert(open_line=="{");

				parent->children.push_back(hierarchy());
				parent->children.back().value=index;
				parse_joint(reader, &parent->children.back());
            }
            else if(key=="End"){
				size_t index = m_joints.size();

                auto open_line=reader.get_line().ltrim();
                assert(open_line=="{");

                auto offsets_line=reader.get_line().ltrim();
                assign_endsite(m_joints[parent->value], offsets_line);

                auto close_line=reader.get_line().ltrim();
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

		m_joints.push_back(joint());
		m_hierarchy.value = 0;

        auto root_line=reader.get_line().ltrim().split();
		auto it = root_line.begin();
		assert(*it == "ROOT");
		++it;
        m_joints[0].name=it->to_str();

        auto open_line=reader.get_line().ltrim();
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
            frames=it->ltrim().to_int();
        }
        {
            auto line=reader.get_line();
            if(!line.startswith("Frame Time:")){
                return false;
            }
        }

		auto channels = 0;
		for (auto it = m_joints.begin(); it != m_joints.end(); ++it){
			channels += it->channels.size();
		}

        m_frames.resize(frames);
        for(size_t i=0; i<frames; ++i){
            auto &frame=m_frames[i];
            auto splited=reader.get_line().split();
			//assert(splited.size() == channels);
			for (auto it = splited.begin(); it != splited.end(); ++it)
            {
                frame.values.push_back(it->to_float());
            }
        }
        return true;
    }
};

} // namespace
} // namespace
} // namespace
