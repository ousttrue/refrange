#pragma once
#include "../reader.h"
#include <string>
#include <vector>
#include <algorithm>


namespace refrange {
namespace binary {
namespace pmx {


struct vec3
{
    float x;
    float y;
    float z;
};


struct vec2
{
    float x;
    float y;
};


struct vertex
{
    vec3 pos;
    vec3 normal;
    vec2 texcoords;
    float edge_factor;
};


class loader
{
    float m_version;
    unsigned char m_flags[8];
    std::wstring m_name;
    std::wstring m_nameEnglish;
    std::wstring m_comment;
    std::wstring m_commentEnglish;

    std::vector<vertex> m_vertices;

public:

    loader()
        : m_version(0)
    {
        m_flags[0]=0;
        m_flags[1]=0;
        m_flags[2]=0;
        m_flags[3]=0;
        m_flags[4]=0;
        m_flags[5]=0;
        m_flags[6]=0;
        m_flags[7]=0;
    }

    std::wstring get_name()const{ return m_name; }
    std::vector<vertex>& get_vertices(){ return m_vertices; }

    bool load(const immutable_range &src)
    {
        range_reader r(src);

        // header
        auto magic=r.read_range(4);
        if(magic!="PMX "){
            return false;
        }

        r.read_value(m_version);

        unsigned char flagbytes;
        r.read_value(flagbytes);
        assert(flagbytes==8);

        r.read(m_flags, 8);

        // model info
        m_name=read_text(r);
        m_nameEnglish=read_text(r);
        m_comment=read_text(r);
        m_commentEnglish=read_text(r);

        // vertices
        int vertex_count;
        r.read_value(vertex_count);
        for(int i=0; i<vertex_count; ++i){
            read_vertex(r);
        }

        return true;
    }

private:
    void read_vertex(range_reader &r)
    {
        m_vertices.push_back(vertex());
        auto &v=m_vertices.back();

        r.read_value(v.pos.x);
        r.read_value(v.pos.y);
        r.read_value(v.pos.z);
        r.read_value(v.normal.x);
        r.read_value(v.normal.y);
        r.read_value(v.normal.z);
        r.read_value(v.texcoords.x);
        r.read_value(v.texcoords.y);

        // additional uv
        assert(m_flags[1]==0);

        unsigned char deform;
        r.read_value(deform);

        switch(deform)
        {
            case 0:
                // BDEF1
                {
                    unsigned int bone_index=read_bone_index(r);
                }
                break;

            case 1:
                // BDEF2
                {
                    unsigned int bone_index_0=read_bone_index(r);
                    unsigned int bone_index_1=read_bone_index(r);
                    float weight0;
                    r.read_value(weight0);
                }
                break;

            case 2:
                // BDEF4
                {
                    assert(false);
                    throw std::invalid_argument(__FUNCTION__);
                }
                break;

            case 3:
                // SDEF
                {
                    assert(false);
                    throw std::invalid_argument(__FUNCTION__);
                }
                break;

            default:
                assert(false);
                throw std::invalid_argument(__FUNCTION__);
        }

        r.read_value(v.edge_factor);
    }

    int read_index(range_reader &r, unsigned int byte_size)
    {
        switch(byte_size)
        {
            case 1:
                {
                    char n;
                    r.read_value(n);
                    return n;
                }
                break;

            case 2:
                {
                    short n;
                    r.read_value(n);
                    return n;
                }
                break;

            case 4:
                {
                    int n;
                    r.read_value(n);
                    return n;
                }
                break;
        }

        assert(false);
        throw std::invalid_argument(__FUNCTION__);
    } 

    int read_bone_index(range_reader &r)
    {
        return read_index(r, m_flags[5]);
    }

    std::wstring read_text(range_reader &r)
    {
        int len;
        r.read_value(len);
        if(len==0){
            return L"";
        }
        auto range=r.read_range(len);

        switch(m_flags[0])
        {
            case 0:
                // utf16
                return std::wstring(
                        (const wchar_t*)range.begin(),
                        (const wchar_t*)range.end()
                        );

            case 1:
                // utf-8
                assert(false);
                return L"";
        }

        assert(false);
        return L"";
    }
};

} // namespace
} // namespace
} // namespace

