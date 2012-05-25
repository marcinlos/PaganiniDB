/*
    Klasa pozwalajaca wczytywac dane z pamieci w przenosny sposob. Do ich
    zapisu sluzy klasa OutputBinaryStream.
*/
#ifndef __PAGANINI_ROW_INPUT_BINARY_STREAM_H__
#define __PAGANINI_ROW_INPUT_BINARY_STREAM_H__

#include <paganini/paging/types.h>
#include <paganini/row/BinaryStream.h>
#include <cstring>
#include <iterator>
#include <iostream>

namespace paganini
{


class InputBinaryStream: public BinaryStream
{
public:
    InputBinaryStream(raw_data buffer, size16 offset = 0);
    
    // Metody statyczne, przy ich pomocy sa implementowane zwykle wersje
    template <typename T>
    static size16 read(const_raw_data buffer, T* output)
    {
        *output = T();
        for (int i = 0; i < sizeof(T); ++ i)
        {
            *output |= ((static_cast<T>(*buffer++) & 0xff) << (i * 8));
        }
        return sizeof(T);
    }
    
    template <typename OutputIter>
    static size16 readRange(const_raw_data buffer, OutputIter out, size16 count)
    {
        size16 sum = 0;
        for (int i = 0; i < count; ++ i)
            sum += read(buffer + sum, &(*(out ++)));
        return sum;
    }
    
    static size16 readData(const_raw_data buffer, raw_data output, 
        size16 length);
    
    // Metody niestatyczne, realizuja swoje funkcje przy uzyciu powyzszych
    // statycznych
    template <typename T>
    size16 read(T* output)
    {
        size16 taken = InputBinaryStream::read(buffer + offset, output);
        offset += taken;
        return taken;
    }
    
    template <typename OutputIter>
    size16 readRange(OutputIter out, size16 count)
    {
        size16 length = InputBinaryStream::readRange(buffer + offset, 
            out, count);
        offset += length;
        return length;
    }
    
    size16 readData(raw_data output, size16 length);
};

template <>
size16 InputBinaryStream::read<float>(const_raw_data buffer, float* output);


}


#endif // __PAGANINI_ROW_INPUT_BINARY_STREAM_H__
