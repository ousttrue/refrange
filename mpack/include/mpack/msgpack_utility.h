#pragma once

namespace mpack {
namespace msgpack {

//////////////////////////////////////////////////////////////////////////////
// utility
//////////////////////////////////////////////////////////////////////////////
inline packer create_external_vector_packer(std::vector<unsigned char> &packed_buffer)
{
    auto buffer=&packed_buffer;
    auto writer=[buffer](const unsigned char *p, size_t size)->size_t
    {
        for(size_t i=0; i<size; ++i){
            buffer->push_back(*p++);
        }
        return size;
    };
    auto pointer=[buffer]()->const unsigned char *{
        return buffer->empty() ? 0 : &((*buffer)[0]);
    };
    auto size=[buffer]()->size_t{
        return buffer->size();
    };
    return packer(writer, pointer, size);
}

inline packer create_vector_packer()
{
    auto buffer=std::make_shared<std::vector<unsigned char>>();
    auto writer=[buffer](const unsigned char *p, size_t size)->size_t
    {
        for(size_t i=0; i<size; ++i){
            buffer->push_back(*p++);
        }
        return size;
    };
    auto pointer=[buffer]()->const unsigned char *{
        return buffer->empty() ? 0 : &((*buffer)[0]);
    };
    auto size=[buffer]()->size_t{
        return buffer->size();
    };
    return packer(writer, pointer, size);
}

struct memory_unpacker
{
    const unsigned char *m_begin;
    const unsigned char *m_p;
    size_t m_size;

    memory_unpacker(const unsigned char *begin, size_t size)
        : m_begin(begin), m_p(m_begin), m_size(size)
    {
    }

    size_t read(unsigned char *p, size_t size)
    {
        if(m_p+size>m_begin+m_size){
            throw std::range_error(__FUNCTION__);
        }

        std::copy(m_p, m_p+size, p);
        m_p+=size;
        return size;
    }
};

inline unpacker create_memory_unpacker(const unsigned char *begin, size_t len)
{
    auto context=std::make_shared<memory_unpacker>(begin, len);
    auto reader=[context](unsigned char *p, size_t size)->size_t
    {
        return context->read(p, size);
    };
    return unpacker(reader);
};

inline unpacker create_unpacker_from_packer(packer &packer)
{
    auto context=std::make_shared<memory_unpacker>(packer.pointer(), packer.size());
    auto reader=[context](unsigned char *p, size_t size)->size_t
    {
        return context->read(p, size);
    };
    return unpacker(reader);
}

} // namespace
} // namespace
