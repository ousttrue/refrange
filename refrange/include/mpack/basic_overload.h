#pragma once

namespace mpack {
namespace msgpack {

using namespace refrange;

//////////////////////////////////////////////////////////////////////////////
// operator<<
//////////////////////////////////////////////////////////////////////////////
// packer
inline packer& operator<<(packer &p, packer &join) { 
    p.write(join.pointer(), join.size());
    return p;
}

// nil
inline packer& operator<<(packer &packer, nil_tag nil) { return packer.pack_nil(); }

// bool
inline packer& operator<<(packer &packer, bool t) { return packer.pack_bool(t); }

// signed
inline packer& operator<<(packer &packer, char t) { return packer.pack_int(t); }
inline packer& operator<<(packer &packer, short t) { return packer.pack_int(t); }
inline packer& operator<<(packer &packer, int t) { return packer.pack_int(t); }
inline packer& operator<<(packer &packer, long long t) { return packer.pack_int(t); }

// unsigned
inline packer& operator<<(packer &packer, unsigned char t) { return packer.pack_int(t); }
inline packer& operator<<(packer &packer, unsigned short t) { return packer.pack_int(t); }
inline packer& operator<<(packer &packer, unsigned int t) { return packer.pack_int(t); }
inline packer& operator<<(packer &packer, unsigned long long t) { return packer.pack_int(t); }

// float
inline packer& operator<<(packer &packer, const float t) { return packer.pack_float(t); }
inline packer& operator<<(packer &packer, const double t) { return packer.pack_double(t); }

// str
inline packer& operator<<(packer &packer, const char *t) { return packer.pack_str(t); }
inline packer& operator<<(packer &packer, const std::string &t){ return packer.pack_str(t.c_str()); }

// bin
inline packer& operator<<(packer &packer, const std::vector<unsigned char> &t){ 
    if(!t.empty()){ packer.pack_bin(&t[0], t.size()); }; return packer;
}

// collection
inline packer& operator<<(packer &packer, const collection_context &t){ return packer.begin_collection(t); }

//////////////////////////////////////////////////////////////////////////////
// operator>>
//////////////////////////////////////////////////////////////////////////////
// bool
inline unpacker& operator>>(unpacker &unpacker, bool &t) { return unpacker.unpack(create_buffer(t)); }
// signed
inline unpacker& operator>>(unpacker &unpacker, char &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, short &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, int &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, long long &t) { return unpacker.unpack(create_buffer(t)); }
// unsigned
inline unpacker& operator>>(unpacker &unpacker, unsigned char &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, unsigned short &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, unsigned int &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, unsigned long long &t) { return unpacker.unpack(create_buffer(t)); }
// float
inline unpacker& operator>>(unpacker &unpacker, float &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, double &t) { return unpacker.unpack(create_buffer(t)); }
// sequence
inline unpacker& operator>>(unpacker &unpacker, std::string &t) { return unpacker.unpack(create_buffer(t)); }
inline unpacker& operator>>(unpacker &unpacker, std::vector<unsigned char> &t) { return unpacker.unpack(create_buffer(t)); }

// collection
inline unpacker& operator>>(unpacker &unpacker, collection_context &c){ return unpacker.unpack(c); }

// only range copy
inline unpacker& operator>>(unpacker &unpacker, immutable_range &r)
{ 
    if(unpacker.is_array()){
        auto begin=unpacker.range().current();
        auto c=array();
        unpacker >> c;
        for(size_t i=0; i<c.size; ++i){
			immutable_range br;
            unpacker >> br;
        }
		r = immutable_range(begin, unpacker.range().current());
    }
    else if(unpacker.is_map()){
        auto begin=unpacker.range().current();
        auto c=array();
        unpacker >> c;
        for(size_t i=0; i<c.size; ++i){
			immutable_range br;
            // key
            unpacker >> br;
            // value
            unpacker >> br;
        }
		r = immutable_range(begin, unpacker.range().current());
    }
    else {
        return unpacker.unpack(create_buffer(r)); 
    }
}

} // namespace
} // namespace
