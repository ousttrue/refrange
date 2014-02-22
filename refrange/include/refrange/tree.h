#pragma once
#include <list>


namespace refrange {

template<typename T, typename Container=std::list<T>>
    struct node
    {
        T value;
        Container children;

        bool operator==(const node<T> &rhs)const
        {
            return value==rhs.value && children==rhs.children;
        }
    };

} // namespace
