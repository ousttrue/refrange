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

    vec3()
        : x(0), y(0), z(0)
    {}
};


struct vec2
{
    float x;
    float y;

    vec2()
        : x(0), y(0)
    {}
};


struct vertex
{
    vec3 pos;
    vec3 normal;
    vec2 texcoords;
    float edge_factor;

    vertex()
        : edge_factor(0)
    {}
};


struct material
{
    std::wstring name;
    std::wstring nameEnglish;
    vec3 diffuse;
    float alpha;
    vec3 specular;
    float specular_factor;
    vec3 ambient;
    unsigned char flag;
    vec3 edge;
    float edge_alpha;
    float edge_size;
    unsigned int diffuse_texture;
    unsigned int sphere_texture;
    unsigned char sphere_mode;
    unsigned char toon_mode;
    unsigned int toon_texture;
    std::wstring comment;
    int vertex_index_count;

    material()
        : alpha(0), specular_factor(0), flag(0)
          , edge_alpha(0), edge_size(0)
          , diffuse_texture(0)
          , sphere_texture(0)
          , sphere_mode(0)
          , toon_mode(0)
          , toon_texture(0)
          , vertex_index_count(0)
	{}
};


struct ik_link
{
    int bone_index;
    unsigned char limited;
    vec3 min;
    vec3 max;

    ik_link()
        : bone_index(0), limited(0)
    {}
};


struct bone
{
    std::wstring name;
    std::wstring nameEnglish;
    vec3 pos;
    int parent_bone;
    int deform_layer;
    unsigned short flag;

    bool flag_connect()const{
        return flag & 0x0001;
    }
    vec3 tail_pos;
    int tail_bone;

    bool flag_reference_rotation()const{
        return (flag & 0x0100)!=0;
    }
    bool flag_reference_translation()const{
        return (flag & 0x0200)!=0;
    }
    int reference_bone;
    float reference_factor;

    bool flag_fixed_axis()const{
        return (flag & 0x0400)!=0;
    }
    vec3 fixed_axis;

    bool flag_local_axis()const{
        return (flag & 0x0800)!=0;
    }
    vec3 local_axis_x;
    vec3 local_axis_z;

    bool flag_external_parent()const{
        return (flag & 0x2000)!=0;
    }
    int external_key;

    bool flag_ik()const{
        return (flag & 0x0020)!=0;
    }
    int ik_target_bone;
    int ik_loop;
    float ik_limit_angle;
    std::vector<ik_link> ik_links;

    bone()
        : parent_bone(-1), deform_layer(0), flag(0)
          , tail_bone(-1)
          , reference_bone(-1), reference_factor(0)
          , external_key(0)
          , ik_target_bone(-1), ik_loop(0), ik_limit_angle(0)
    {}
};


struct morph_vertex
{
    int vertex_index;
    vec3 offset;
};


struct morph
{
    std::wstring name;
    std::wstring nameEnglish;
    unsigned char panel;
    unsigned char type;

    std::vector<morph_vertex> vertices;

    morph()
        : panel(0), type(0)
    {}
};


struct group
{
    std::wstring name;
    std::wstring nameEnglish;
    unsigned char flag;

    std::vector<int> bones;
    std::vector<int> morphs;
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


class loader
{
    float m_version;
    unsigned char m_flags[8];
    std::wstring m_name;
    std::wstring m_nameEnglish;
    std::wstring m_comment;
    std::wstring m_commentEnglish;

    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<std::wstring> m_textures;
    std::vector<material> m_materials;
    std::vector<bone> m_bones;
    std::vector<morph> m_morphs;
    std::vector<group> m_groups;
	std::vector<rigidbody> m_rigidbodies;
    std::vector<joint> m_joints;

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
    std::vector<unsigned int>& get_indices(){ return m_indices; }
    std::vector<std::wstring>& get_textures(){ return m_textures; }
    std::vector<material>& get_materials(){ return m_materials; }
    std::vector<bone>& get_bones(){ return m_bones; }
    std::vector<morph>& get_morphs(){ return m_morphs; }
    std::vector<group>& get_groups(){ return m_groups; }
    std::vector<rigidbody>& get_rigidbodies(){ return m_rigidbodies; }
    std::vector<joint>& get_joints(){ return m_joints; }

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

        // indices
        int index_count;
        r.read_value(index_count);
        m_indices.resize(index_count);
        auto index_size=m_flags[2];
        auto range=r.read_range(index_count * index_size);
        switch(index_size)
        {
            case 1:
                std::copy(
                        (const unsigned char*)range.begin(),
                        (const unsigned char*)range.end(),
                        m_indices.begin()
                        );
                break;
            case 2:
                break;
            case 4:
                break;

            default:
                throw std::invalid_argument(__FUNCTION__);
        }

        // textures
        int texture_count;
        r.read_value(texture_count);
        for(int i=0; i<texture_count; ++i){
			m_textures.push_back(read_text(r));
        }

        // mterials
        int material_count;
        r.read_value(material_count);
        for(int i=0; i<material_count; ++i){
            read_material(r);
        }

        // bones
        int bone_count;
        r.read_value(bone_count);
        for(int i=0; i<bone_count; ++i){
            read_bone(r);
        }

        // morphs
        int morph_count;
        r.read_value(morph_count);
        for(int i=0; i<morph_count; ++i){
            read_morph(r);
        }

        // group
        int group_count;
        r.read_value(group_count);
        for(int i=0; i<group_count; ++i){
            m_groups.push_back(group());
            auto &g=m_groups.back();

            g.name=read_text(r);
            g.nameEnglish=read_text(r);
            r.read_value(g.flag);

            int num;
            r.read_value(num);
            for(int i=0; i<num; ++i){
                unsigned char type;
                r.read_value(type);

                switch(type)
                {
                    case 0:
                        g.bones.push_back(read_bone_index(r));
                        break;

                    case 1:
                        g.morphs.push_back(read_morph_index(r));
                        break;

                    default:
                        throw std::invalid_argument(__FUNCTION__);
                }
            }
        }

        // rigid body
        int rigidbody_count;
        r.read_value(rigidbody_count);
        for(int i=0; i<rigidbody_count; ++i){
            read_rigidbody(r);
        }

		// joint
        int joint_count;
        r.read_value(joint_count);
        for(int i=0; i<joint_count; ++i){
            read_joint(r);
        }

        return true;
    }

private:
    void read_joint(range_reader &r)
    {
        m_joints.push_back(joint());
        auto &j=m_joints.back();

        j.name=read_text(r);
        j.nameEnglish=read_text(r);
        r.read_value(j.type);
        switch(j.type)
        {
            case 0:
                {
                    // 6DOF
                    j.rigidbody_a=read_rigidbody_index(r);
                    j.rigidbody_b=read_rigidbody_index(r);
                    r.read_value(j.pos.x);
                    r.read_value(j.pos.y);
                    r.read_value(j.pos.z);
                    r.read_value(j.rot.x);
                    r.read_value(j.rot.y);
                    r.read_value(j.rot.z);
                    r.read_value(j.translation_min.x);
                    r.read_value(j.translation_min.y);
                    r.read_value(j.translation_min.z);
                    r.read_value(j.translation_max.x);
                    r.read_value(j.translation_max.y);
                    r.read_value(j.translation_max.z);
                    r.read_value(j.rotation_min.x);
                    r.read_value(j.rotation_min.y);
                    r.read_value(j.rotation_min.z);
                    r.read_value(j.rotation_max.x);
                    r.read_value(j.rotation_max.y);
                    r.read_value(j.rotation_max.z);
                    r.read_value(j.spring_translation.x);
                    r.read_value(j.spring_translation.y);
                    r.read_value(j.spring_translation.z);
                    r.read_value(j.spring_rotation.x);
                    r.read_value(j.spring_rotation.y);
                    r.read_value(j.spring_rotation.z);
                }
                break;

            default:
                throw std::invalid_argument(__FUNCTION__);
        }
    }

    void read_rigidbody(range_reader &r)
    {
        m_rigidbodies.push_back(rigidbody());
        auto &b=m_rigidbodies.back();

        b.name=read_text(r);
        b.nameEnglish=read_text(r);
        b.bone_index=read_bone_index(r);
        r.read_value(b.group);
        r.read_value(b.mask);
        r.read_value(b.shape_type);
        r.read_value(b.shape_size.x);
        r.read_value(b.shape_size.y);
        r.read_value(b.shape_size.z);
        r.read_value(b.shape_pos.x);
        r.read_value(b.shape_pos.y);
        r.read_value(b.shape_pos.z);
        r.read_value(b.shape_rot.x);
        r.read_value(b.shape_rot.y);
        r.read_value(b.shape_rot.z);
        r.read_value(b.mass);
        r.read_value(b.linear_damping);
        r.read_value(b.anglar_damping);
        r.read_value(b.restitution);
        r.read_value(b.friction);
        r.read_value(b.type);
    }

    void read_morph_vertex(range_reader &r, morph &m, int count)
    {
        m.vertices.reserve(count);
        for(int i=0; i<count; ++i){
            m.vertices.push_back(morph_vertex());
			auto &v=m.vertices.back();

            v.vertex_index=read_vertex_index(r);
            r.read_value(v.offset.x);
            r.read_value(v.offset.y);
            r.read_value(v.offset.z);
        }
    }

    void read_morph(range_reader &r)
    {
        m_morphs.push_back(morph());
        auto &m=m_morphs.back();

        m.name=read_text(r);
        m.nameEnglish=read_text(r);
        r.read_value(m.panel);
        r.read_value(m.type);
        
        int morph_count;
        r.read_value(morph_count);

        switch(m.type)
        {
            case 0:
                // group
                throw std::invalid_argument(__FUNCTION__);

            case 1:
                // vertex
                read_morph_vertex(r, m, morph_count);
                break;

            case 2:
                // bone
                throw std::invalid_argument(__FUNCTION__);

            case 3:
                // uv
                throw std::invalid_argument(__FUNCTION__);

            case 4:
                // uv + 1
                throw std::invalid_argument(__FUNCTION__);

            case 5:
                // uv + 2
                throw std::invalid_argument(__FUNCTION__);

            case 6:
                // uv + 3
                throw std::invalid_argument(__FUNCTION__);

            case 7:
                // uv + 4
                throw std::invalid_argument(__FUNCTION__);

            case 8:
                // material
                throw std::invalid_argument(__FUNCTION__);

            default:
                throw std::invalid_argument(__FUNCTION__);
        }
    }

    void read_bone(range_reader &r)
    {
        m_bones.push_back(bone());
        auto &b=m_bones.back();

        b.name=read_text(r);
        b.nameEnglish=read_text(r);
        r.read_value(b.pos.x);
        r.read_value(b.pos.y);
        r.read_value(b.pos.z);
        b.parent_bone=read_bone_index(r);
        r.read_value(b.deform_layer);
        r.read_value(b.flag);

        if(b.flag_connect()){
            b.tail_bone=read_bone_index(r);
        }
        else{
            r.read_value(b.tail_pos.x);
            r.read_value(b.tail_pos.y);
            r.read_value(b.tail_pos.z);
        }

        if(b.flag_reference_rotation() || b.flag_reference_translation())
        {
            b.reference_bone=read_bone_index(r);
            r.read_value(b.reference_factor);
        }

        if(b.flag_fixed_axis()){
            r.read_value(b.fixed_axis.x);
            r.read_value(b.fixed_axis.y);
            r.read_value(b.fixed_axis.z);
        }

        if(b.flag_local_axis()){
            r.read_value(b.local_axis_x.x);
            r.read_value(b.local_axis_x.y);
            r.read_value(b.local_axis_x.z);
            r.read_value(b.local_axis_z.x);
            r.read_value(b.local_axis_z.y);
            r.read_value(b.local_axis_z.z);
        }

        if(b.flag_external_parent()){
            r.read_value(b.external_key);
        }

        if(b.flag_ik()){
            b.ik_target_bone=read_bone_index(r);
            r.read_value(b.ik_loop);
            r.read_value(b.ik_limit_angle);
            int ik_links;
            r.read_value(ik_links);
            for(int i=0; i<ik_links; ++i){
                b.ik_links.push_back(ik_link());
                auto &link=b.ik_links.back();

				link.bone_index=read_bone_index(r);
                r.read_value(link.limited);
                if(link.limited){
                    r.read_value(link.min.x);
                    r.read_value(link.min.y);
                    r.read_value(link.min.z);
                    r.read_value(link.max.x);
                    r.read_value(link.max.y);
                    r.read_value(link.max.z);
                }
            }
        }
    }

    void read_material(range_reader &r)
    {
        m_materials.push_back(material());
        auto &m=m_materials.back();

        m.name=read_text(r);
        m.nameEnglish=read_text(r);
        r.read_value(m.diffuse.x);
        r.read_value(m.diffuse.y);
        r.read_value(m.diffuse.z);
        r.read_value(m.alpha);
        r.read_value(m.specular.x);
        r.read_value(m.specular.y);
        r.read_value(m.specular.z);
        r.read_value(m.specular_factor);
        r.read_value(m.ambient.x);
        r.read_value(m.ambient.y);
        r.read_value(m.ambient.z);
        r.read_value(m.flag);
        r.read_value(m.edge.x);
        r.read_value(m.edge.y);
        r.read_value(m.edge.z);
        r.read_value(m.edge_alpha);
        r.read_value(m.edge_size);
        m.diffuse_texture=read_texture_index(r);
        m.sphere_texture=read_texture_index(r);
        r.read_value(m.sphere_mode);
        r.read_value(m.toon_mode);
        if(m.toon_mode==0){
            m.toon_texture=read_texture_index(r);
        }
        else{
            unsigned char toon_index;
            r.read_value(toon_index);
            m.toon_texture=toon_index;
        }
        m.comment=read_text(r);
        r.read_value(m.vertex_index_count);
    }

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
                    unsigned int bone_index_0=read_bone_index(r);
                    unsigned int bone_index_1=read_bone_index(r);
                    unsigned int bone_index_2=read_bone_index(r);
                    unsigned int bone_index_3=read_bone_index(r);
                    float weight0;
                    r.read_value(weight0);
                    float weight1;
                    r.read_value(weight1);
                    float weight2;
                    r.read_value(weight2);
                    float weight3;
                    r.read_value(weight3);
                }
                break;

            case 3:
                // SDEF
                {
                    unsigned int bone_index_0=read_bone_index(r);
                    unsigned int bone_index_1=read_bone_index(r);
                    float weight0;
                    r.read_value(weight0);
                    vec3 c;
                    r.read_value(c.x);
                    r.read_value(c.y);
                    r.read_value(c.z);
                    vec3 r0;
                    r.read_value(r0.x);
                    r.read_value(r0.y);
                    r.read_value(r0.z);
                    vec3 r1;
                    r.read_value(r1.x);
                    r.read_value(r1.y);
                    r.read_value(r1.z);
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

    int read_vertex_index(range_reader &r) { return read_index(r, m_flags[2]); }
    int read_texture_index(range_reader &r) { return read_index(r, m_flags[3]); }
    int read_material_index(range_reader &r) { return read_index(r, m_flags[4]); }
    int read_bone_index(range_reader &r) { return read_index(r, m_flags[5]); }
    int read_morph_index(range_reader &r) { return read_index(r, m_flags[6]); }
    int read_rigidbody_index(range_reader &r) { return read_index(r, m_flags[7]); }

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

