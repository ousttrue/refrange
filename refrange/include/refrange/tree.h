#pragma once
#include <list>


namespace refrange {

template<typename T>
    struct node
    {
        T value;
		std::list<node<T>> children;

        bool operator==(const node<T> &rhs)const
        {
            return value==rhs.value && children==rhs.children;
        }
    };

} // namespace
