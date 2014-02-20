#pragma once

namespace mpack{
namespace msgpack{

enum typecategory_t
{
    typecategory_unknown,
    typecategory_null,
    typecategory_bool,
    typecategory_int,
    typecategory_float,
    typecategory_byte_array,
    typecategory_string,
    typecategory_collection,
};

inline typecategory_t typecategory(unsigned char b)
{
    if(positive_fixint_tag::is_match(b)){
        return typecategory_int;
    }
	if (negative_fixint_tag::is_match(b)){
        return typecategory_int;
    }
	if (fixstr_tag::is_match(b)){
        return typecategory_string;
    }

    switch(b)
    {
        case nil_tag::bits: 
            return typecategory_null;

        case false_tag::bits:
        case true_tag::bits:
            return typecategory_bool;

        case bin8_tag::bits:
        case bin16_tag::bits:
        case bin32_tag::bits:
            return typecategory_byte_array;

        case float32_tag::bits:
        case float64_tag::bits:
            return typecategory_float;

        case uint8_tag::bits:
        case uint16_tag::bits:
        case uint32_tag::bits:
        case uint64_tag::bits:
        case int8_tag::bits:
        case int16_tag::bits:
        case int32_tag::bits:
        case int64_tag::bits:
            return typecategory_int;

        case str8_tag::bits:
        case str16_tag::bits:
        case str32_tag::bits:
            return typecategory_string;

        case array16_tag::bits:
        case array32_tag::bits:
        case map16_tag::bits:
        case map32_tag::bits:
            return typecategory_collection;

            /*
        case ext8_tag::bits:
        case ext16_tag::bits:
        case ext32_tag::bits:
        case fixext1_tag::bits:
        case fixext2_tag::bits:
        case fixext4_tag::bits:
        case fixext8_tag::bits:
        case fixext16_tag::bits:
            */
    }

    return typecategory_unknown;
}

/*
inline void drop(unpacker &u)
{
	auto category=typecategory(u.peek_byte());
    switch(category)
    {
        case typecategory_bool:
            {
                bool b;
                u >> b;
            }
            break;

        case typecategory_int:
            {
                int n;
                u >> n;
            }
            break;

        case typecategory_float:
            {
                float n;
                u >> n;
            }
            break;

        case typecategory_byte_array:
            {
                std::vector<unsigned char> bin;
                u >> bin;
            }
            break;

		case typecategory_string:
			{
				std::string s;
				u >> s;
			}
			break;

        default:
			throw std::exception(__FUNCTION__);
            break;
    }
}
*/

inline void typestruct(unpacker &u, std::ostream &os)
{
    if(u.is_array()){
        os << "[";
        auto c=array();
        u >> c;
        for(size_t i=0; i<c.size; ++i){
            if(i){
                os << ",";
            }
            typestruct(u,  os);
        }
        os << "]";
    }
    else if(u.is_map()){
        os << "{";
        auto c=map();
        u >> c;
        for(size_t i=0; i<c.size; ++i){
            if(i){
                os << ",";
            }
            typestruct(u,  os);
            os << ":";
            typestruct(u,  os);
        }
        os << "}";
    }
    else{
        if(u.is_bool()){
            os << "bool";
        }
        else if(u.is_integer()){
            os << "int";
        }
        else if(u.is_float()){
            os << "float";
        }
        else if(u.is_bin()){
            os << "byte[]";
        }
        else if(u.is_str()){
            os << "string";
        }
        else{
            os << "unknown";
        }

        u.drop();
    }
}

} // namespace
} // namespace
