/*
    Proste klasy reprezentujaca bufor na dane
*/
#ifndef __PAGANINI_NETWORKING_BUFFERS_H__
#define __PAGANINI_NETWORKING_BUFFERS_H__

#include <cstddef>
#include <array>
#include <vector>
#include <string>
using std::string;


namespace paganini
{
namespace networking
{

namespace details
{
    const void* offset_bytes(const void* begin, std::size_t offset)
    {
        return reinterpret_cast<const void*>(
            reinterpret_cast<const char*>(begin) + offset); 
    }
    
    void* offset_bytes(void* begin, std::size_t offset)
    {
        return reinterpret_cast<void*>(reinterpret_cast<char*>(begin) + offset); 
    }
}


struct ConstBuffer
{
    const void* const data;
    const std::size_t size;
    
    // Pusty bufor
    ConstBuffer(): data(nullptr), size(0) 
    {
    }
    
    // Bufor z zakresu pamieci
    ConstBuffer(const void* data, std::size_t size): data(data), size(size)
    {
    }
    
    ConstBuffer offset(std::size_t offset) const
    {
        return { details::offset_bytes(data, offset), size - offset };
    }
};


struct Buffer
{
    void* const data;
    const std::size_t size;
    
    // Pusty bufor
    Buffer(): data(nullptr), size(0) 
    {
    }
    
    // Bufor z zakresu pamieci
    Buffer(void* data, std::size_t size): data(data), size(size)
    {
    }

    // Konwersja na wersje const
    operator ConstBuffer() const
    {
        return { data, size };
    }
    
    Buffer offset(std::size_t offset) const
    {
        return { details::offset_bytes(data, offset), size - offset };
    }
};


// Funkcje tworzace bufory z podanych konstrukcji

ConstBuffer make_buffer(const void* data, std::size_t size)
{
    return { data, size };
}


Buffer make_buffer(void* data, std::size_t size)
{
    return { data, size };
}


template <typename T, int N>
ConstBuffer make_buffer(const T (&data)[N])
{
    return { data, N * sizeof(T) };
}


template <typename T, int N>
Buffer make_buffer(T (&data)[N])
{
    return { data, N * sizeof(T) };
}


template <typename T, int N>
ConstBuffer make_buffer(const T (&data)[N], std::size_t n)
{
    return { data, n * sizeof(T) };
}


template <typename T, int N>
Buffer make_buffer(T (&data)[N], std::size_t n)
{
    return { data, n * sizeof(T) };
}


template <typename T, int N>
ConstBuffer make_buffer(std::array<T, N> data)
{
    return { data, N * sizeof(T) };
}


template <typename T, int N>
Buffer make_buffer(std::array<T, N> data)
{
    return { data, N * sizeof(T) };
}

template <typename T, int N>
ConstBuffer make_buffer(std::array<T, N> data, std::size_t n)
{
    return { data, n * sizeof(T) };
}


template <typename T, int N>
Buffer make_buffer(std::array<T, N> data, std::size_t n)
{
    return { data, n * sizeof(T) };
}


template <typename T>
ConstBuffer make_buffer(const std::vector<T>& data)
{
    return { &data[0], data.size() * sizeof(T) };
}


template <typename T>
Buffer make_buffer(std::vector<T>& data)
{
    return { &data[0], data.size() * sizeof(T) };
}


template <typename T>
ConstBuffer make_buffer(const std::vector<T>& data, std::size_t n)
{
    return { &data[0], n * sizeof(T) };
}


template <typename T>
Buffer make_buffer(std::vector<T>& data, std::size_t n)
{
    return { &data[0], n * sizeof(T) };
}


ConstBuffer make_buffer(const string& data)
{
    return { &data[0], data.size() };
}


Buffer make_buffer(string& data)
{
    return { &data[0], data.size() };
}


ConstBuffer make_buffer(const string& data, std::size_t n)
{
    return { &data[0], n };
}


Buffer make_buffer(string& data, std::size_t n)
{
    return { &data[0], n };
}


} // networking
} // paganini


#endif // __PAGANINI_NETWORKING_BUFFERS_H__

