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
    if(partial_bit_equal<positive_fixint>(b)){
        return typecategory_int;
    }
    if(partial_bit_equal<negative_fixint>(b)){
        return typecategory_int;
    }
    if(partial_bit_equal<fixstr>(b)){
        return typecategory_string;
    }

    switch(b)
    {
        case byte_nil: 
            return typecategory_null;

        case byte_false:
        case byte_true:
            return typecategory_bool;

        case byte_bin8:
        case byte_bin16:
        case byte_bin32:
            return typecategory_byte_array;

        case byte_float32:
        case byte_float64:
            return typecategory_float;

        case byte_uint8:
        case byte_uint16:
        case byte_uint32:
        case byte_uint64:
        case byte_int8:
        case byte_int16:
        case byte_int32:
        case byte_int64:
            return typecategory_int;

        case byte_str8:
        case byte_str16:
        case byte_str32:
            return typecategory_string;

        case byte_array16:
        case byte_array32:
        case byte_map16:
        case byte_map32:
            return typecategory_collection;

            /*
        case byte_ext8:
        case byte_ext16:
        case byte_ext32:
        case byte_fixext1:
        case byte_fixext2:
        case byte_fixext4:
        case byte_fixext8:
        case byte_fixext16:
            */
    }

    return typecategory_unknown;
}

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
        switch(typecategory(u.peek_byte()))
        {
            case typecategory_bool:
                os << "bool";
                break;

            case typecategory_int:
                os << "int";
                break;

            case typecategory_float:
                os << "float";
                break;

            case typecategory_byte_array:
                os << "byte[]";
                break;

			case typecategory_string:
				os << "string";
				break;

			default:
                os << "unknown";
                break;
        }
        drop(u);
    }
}

} // namespace
} // namespace
