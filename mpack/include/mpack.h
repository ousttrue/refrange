#pragma once
#include <vector>


namespace mpack
{

    class packer
    {
    public:
        std::vector<unsigned char> packed_buffer;

        template<typename T>
            packer& operator<<(const T &t)
            {
                return *this;
            }
    };

    class reference_unpacker
    {
        const unsigned char *m_begin;
        size_t m_size;

    public:
        reference_unpacker(const unsigned char*p, size_t size)
            : m_begin(p), m_size(size)
        {
        };

        template<typename T>
            bool unpack(T &t)
            {
                return false;
            }
    };

};

