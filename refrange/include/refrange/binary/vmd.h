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


struct frame
{
    std::string bonename;
    unsigned int framenum;
    vec3 pos;
    quaternion rot;
    char interpolate[64];

    bool operator<(const frame &rhs)const
    {
        return framenum<rhs.framenum;
    }
};


class loader
{
	std::string m_modelname;
    std::vector<frame> m_frames;
    unsigned int m_maxframenum;

public:
    loader()
        : m_maxframenum(0)
    {}

    std::string get_modelname()const{ return m_modelname; }
    std::vector<frame> &get_frames(){ return m_frames; }
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

            m_frames.resize(motionCount);
			for (unsigned int i = 0; i < motionCount; ++i){
				auto &f = m_frames[i];

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

        // camera

        // light

        // self shadow

        return true;
    }
};

} // namespace
} // namespace
} // namespace

