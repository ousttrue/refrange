#pragma once
#include <list>


namespace refrange {

template<typename T>
    struct node
    {
        T value;
		std::list<node<T>> children;
        node<T> *parent;

        node()
            : parent(0)
        {
        }

        node(const T &t)
            : value(t), parent(0)
        {
        }

        bool operator==(const node<T> &rhs)const
        {
            return value==rhs.value && children==rhs.children;
        }

        node<T> *find(const T &key)
        {
            if(value==key){
                return this;
            }
            for(auto it=children.begin(); it!=children.end(); ++it){
                auto found=it->find(key);
                if(found){
                    return found;
                }
            }
            return 0;
        }

        void add_child(node<T> &child)
        {
            child.parent=this;
            children.push_back(child);
        }
    };

} // namespace
