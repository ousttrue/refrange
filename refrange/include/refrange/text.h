#pragma once
#include <ctype.h>


namespace refrange {
namespace text {


template<typename T>
inline bool is_space(T p)
{
    return isspace(*p) != 0;
}


template<typename T>
inline bool is_digit(T p)
{
    return isdigit(*p) != 0;
}


} // namespace
} // namespace
