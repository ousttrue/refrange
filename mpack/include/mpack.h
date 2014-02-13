#pragma once
#include <vector>


namespace mpack
{

    class packer
    {
        public:
            std::vector<unsigned char> packed_buffer;
    };

    template<typename T>
        packer& operator<<(packer &packer, const T &t)
        {
            // not implmented
            throw std::exception();
        }

    // int
    packer& operator<<(packer &packer, int n)
    {
        packer.packed_buffer.push_back(n);
        return packer;
    };

    class reference_unpacker
    {
        const unsigned char *m_begin;
        const unsigned char *m_p;
        size_t m_size;

        public:
        reference_unpacker(const unsigned char*p, size_t size)
            : m_begin(p), m_p(m_begin), m_size(size)
        {
        };

        int get_int()
        {
            if(m_p>=m_begin+m_size){
                // over
                throw std::exception();
            }
            unsigned char n=*m_p++;
            return static_cast<int>(n);
        }
    };

    template<typename T>
        reference_unpacker& operator>>(reference_unpacker &unpacker, const T &t)
        {
            // not implmented
            throw std::exception();
        }

    // int
    reference_unpacker& operator>>(reference_unpacker &unpacker, int &n)
    {
        n=unpacker.get_int();
        return unpacker;
    }

};

