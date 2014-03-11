#pragma once
#include "../reader.h"
#include <string>
#include <vector>
#include <algorithm>


namespace refrange {
namespace binary {
namespace pmd {


struct vec2
{
    float x;
    float y;
};


struct vec3
{
    float x;
    float y;
    float z;
};


struct vertex
{
    vec3 pos;
    vec3 normal;
    vec2 uv;
    unsigned short bone0;
    unsigned short bone1;
    unsigned char weight0;
    unsigned char flag;
};


struct material
{
    vec3 diffuse;
    float alpha;
    float specular_factor;
    vec3 specular;
    vec3 ambient;
    unsigned char toon_index;
    unsigned char flag;
    unsigned int face_vertex_count;
    std::string texture;
};


struct bone
{
    std::string name;
    unsigned short parent_bone;
    unsigned short tail_bone;
    unsigned char bone_type;
    unsigned short ik_bone;
    vec3 pos;
};


struct morph
{
};


struct rigidbody
{
    int bone_index;
    std::wstring name;
    std::wstring nameEnglish;
    unsigned char group;
    unsigned short mask;
    unsigned char shape_type;
    vec3 shape_size;
    vec3 shape_pos;
    vec3 shape_rot;
    float mass;
    float linear_damping;
    float anglar_damping;
    float restitution;
    float friction;
    unsigned char type;

    rigidbody()
        : bone_index(-1), group(0), mask(0), shape_type(0)
          , mass(0), linear_damping(0), anglar_damping(0)
          , restitution(0), friction(0), type(0)
    {}
};


struct joint
{
    std::wstring name;
    std::wstring nameEnglish;
    int rigidbody_a;
    int rigidbody_b;
    unsigned char type;
    vec3 pos;
    vec3 rot;
    vec3 translation_min;
    vec3 translation_max;
    vec3 rotation_min;
    vec3 rotation_max;
    vec3 spring_translation;
    vec3 spring_rotation;
};


struct ik
{
    unsigned short target_bone;
    unsigned short effector_bone;
    unsigned short iteration;
    float limit_angle;
    std::vector<unsigned short> children;
};


class pmd_range_reader: public range_reader
{
public:
    pmd_range_reader(const immutable_range &src)
        : range_reader(src)
    {}

    float read_float()
    {
        float v;
        read_value(v);
        return v;
    }

    vec2 read_vec2()
    {
        vec2 v;
        read_value(v);
        return v;
    }

    vec3 read_vec3()
    {
        vec3 v;
        read_value(v);
        return v;
    }

    /*
    vec4 read_vec4()
    {
        vec4 v;
        read_value(v);
        return v;
    }
    */
};


class loader
{
    std::string m_name;
    std::string m_comment;
    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    //std::vector<std::wstring> m_textures;
    std::vector<material> m_materials;
    std::vector<bone> m_bones;
    std::vector<ik> m_iklist;
    std::vector<morph> m_morphs;
    //std::vector<group> m_groups;
	std::vector<rigidbody> m_rigidbodies;
    std::vector<joint> m_joints;

public:

    bool load(const immutable_range &src)
    {
        pmd_range_reader r(src);

        auto sig=r.read_range(3);
        if(sig!="Pmd"){
            return false;
        }

        float version;
        r.read_value(version);
        if(version!=1.0f){
            return false;
        }

        m_name=r.read_range(20).to_str();
        m_comment=r.read_range(256).to_str();

        // vertices
        unsigned int vertex_count;
        r.read_value(vertex_count);
        m_vertices.resize(vertex_count);
        for(size_t i=0; i<vertex_count; ++i){
            auto &v=m_vertices[i];
            r.read_value(v.pos);
            r.read_value(v.normal);
            r.read_value(v.uv);
            r.read_value(v.bone0);
            r.read_value(v.bone1);
            r.read_value(v.weight0);
            r.read_value(v.flag);
        }

        // indices
        unsigned int index_count;
        r.read_value(index_count);
        m_indices.resize(index_count);
        r.read((unsigned char*)&m_indices[0], index_count * sizeof(unsigned short));

        // materials
        unsigned int material_count;
        r.read_value(material_count);
        m_materials.resize(material_count);
        for(size_t i=0; i<material_count; ++i){
            auto &m=m_materials[i];
            r.read_value(m.diffuse);
            r.read_value(m.alpha);
            r.read_value(m.specular_factor);
            r.read_value(m.specular);
            r.read_value(m.ambient);
            r.read_value(m.toon_index);
            r.read_value(m.flag);
            r.read_value(m.face_vertex_count);
            m.texture=r.read_range(20).to_str();
        }

        // bone
        unsigned short bone_count;
        r.read_value(bone_count);
        m_bones.resize(bone_count);
        for(size_t i=0; i<bone_count; ++i){
            auto &b=m_bones[i];
            b.name=r.read_range(20).to_str();
            r.read_value(b.parent_bone);
            r.read_value(b.tail_bone);
            r.read_value(b.bone_type);
            r.read_value(b.ik_bone);
            r.read_value(b.pos);
        }

        // ik
        unsigned short ik_count;
        r.read_value(ik_count);
        m_iklist.resize(ik_count);
        for(size_t i=0; i<ik_count; ++i){
            auto &ik=m_iklist[i];
            r.read_value(ik.target_bone);
            r.read_value(ik.effector_bone);

            unsigned char length;
            r.read_value(length);
            ik.children.resize(length);

            r.read_value(ik.iteration);
            r.read_value(ik.limit_angle);

            for(size_t i=0; i<length; ++i){
                r.read_value(ik.children[i]);
            }
        }

        // morph

        return true;
    }

    std::string get_name()const{ return m_name; }
    std::vector<vertex>& get_vertices(){ return m_vertices; }
    std::vector<unsigned int>& get_indices(){ return m_indices; }
    std::vector<material>& get_materials(){ return m_materials; }
    std::vector<bone>& get_bones(){ return m_bones; }
    std::vector<ik>& get_iklist(){ return m_iklist; }
    std::vector<morph>& get_morphs(){ return m_morphs; }
    //std::vector<group>& get_groups(){ return m_groups; }
    std::vector<rigidbody>& get_rigidbodies(){ return m_rigidbodies; }
    std::vector<joint>& get_joints(){ return m_joints; }
};


} // namespace
} // namespace
} // namespace
