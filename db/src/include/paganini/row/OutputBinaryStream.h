/*
    Klasa pozwalajaca zapisywac dane do pamieci w przenosny sposob. Do ich
    odczytywania slozy inna klasa, InputBinaryStream.
*/
#ifndef __PAGANINI_ROW_OUTPUT_BINARY_STREAM_H__
#define __PAGANINI_ROW_OUTPUT_BINARY_STREAM_H__

#include <paganini/paging/types.h>
#include <paganini/row/BinaryStream.h>
#include <cstring>

namespace paganini
{


class OutputBinaryStream: public BinaryStream
{   
public:
    OutputBinaryStream(raw_data buffer, size16 offset = 0);
    
    // Metody statyczne, przy ich pomocy sa implementowane zwykle wersje
    template <typename T>
    static size16 write(raw_data buffer, T value)
    {
        for (int i = 0; i < sizeof(value); ++ i)
        {
            *buffer++ = (value & 0xff);
            value >>= 8;
        }
        return sizeof(value);
    }
    
    template <typename Iter>
    static size16 writeRange(raw_data buffer, Iter begin, Iter end)
    {
        size16 sum = 0;
        while (begin != end)
            sum += write(buffer + sum, *begin ++);
        return sum;
    }
    
    static size16 writeData(raw_data buffer, const_raw_data data, 
        size16 length);
    
    // Metody niestatyczne, realizuja swoje funkcje przy uzyciu powyzszych
    // statycznych
    template <typename T>
    size16 write(T value)
    {
        size16 written = OutputBinaryStream::write(buffer + offset, value);
        offset += written;
        return written;
    }
    
    template <typename Iter>
    size16 writeRange(Iter begin, Iter end)
    {
        size16 length = OutputBinaryStream::writeRange(buffer + offset, 
            begin, end);
        offset += length;
        return length;
    }
    
    size16 writeData(const_raw_data data, size16 length);
};


template <>
size16 OutputBinaryStream::write<float>(raw_data buffer, float value);


}


#endif // __PAGANINI_ROW_OUTPUT_BINARY_STREAM_H__
