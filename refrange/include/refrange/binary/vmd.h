#pragma once
#include "../reader.h"
#include <string>
#include <vector>
#include <algorithm>


namespace refrange {
namespace binary {
namespace vmd {


struct vec3
{
    float x;
    float y;
    float z;
};


struct quaternion
{
    float x;
    float y;
    float z;
    float w;
};


struct boneframe
{
    std::string bonename;
    unsigned int framenum;
    vec3 pos;
    quaternion rot;
    char interpolate[64];

    bool operator<(const boneframe &rhs)const
    {
        return framenum<rhs.framenum;
    }
};


struct morphframe
{
	std::string morphname;
	unsigned int framenum;
	float value;

    bool operator<(const morphframe &rhs)const
    {
        return framenum<rhs.framenum;
    }
};


class loader
{
	std::string m_modelname;
    std::vector<boneframe> m_boneframes;
	std::vector<morphframe> m_morphframes;
    unsigned int m_maxframenum;

public:
    loader()
        : m_maxframenum(0)
    {}

    std::string get_modelname()const{ return m_modelname; }
    std::vector<boneframe> &get_boneframes(){ return m_boneframes; }
	std::vector<morphframe> &get_morphframes(){ return m_morphframes; }
    unsigned int get_maxframenum(){ return m_maxframenum; }

    bool load(const immutable_range &src)
    {
        range_reader r(src);

        {
            auto header=r.read_str(30);
            if(header!="Vocaloid Motion Data 0002"){
                return false;
            }
        }

		{
			m_modelname = r.read_str(20);
		}

        // motion
        {
            unsigned int motionCount;
            r.read_value(motionCount);

            m_boneframes.resize(motionCount);
			for (unsigned int i = 0; i < motionCount; ++i){
				auto &f = m_boneframes[i];

				f.bonename = r.read_str(15);
				r.read_value(f.framenum);
                m_maxframenum=std::max(m_maxframenum, f.framenum);
				// pos
				r.read_value(f.pos.x);
				r.read_value(f.pos.y);
				r.read_value(f.pos.z);
				// rot
				r.read_value(f.rot.x);
				r.read_value(f.rot.y);
				r.read_value(f.rot.z);
				r.read_value(f.rot.w);
				// interpolate
				r.read((unsigned char*)f.interpolate, 64);
			}
        }

        // morph
		{
            unsigned int frameCount;
            r.read_value(frameCount);

            m_morphframes.resize(frameCount);
			for (unsigned int i = 0; i < frameCount; ++i){
				auto &f = m_morphframes[i];

				f.morphname = r.read_str(15);
				r.read_value(f.framenum);
                m_maxframenum=std::max(m_maxframenum, f.framenum);
				r.read_value(f.value);
			}
		}

        // camera

        // light

        // self shadow

        return true;
    }
};

} // namespace
} // namespace
} // namespace

