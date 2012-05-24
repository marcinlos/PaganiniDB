#include "config.h"
#include <paganini/row/InputBinaryStream.h>

namespace paganini
{


InputBinaryStream::InputBinaryStream(raw_data buffer, size16 offset): 
    BinaryStream(buffer, offset)
{
}


size16 InputBinaryStream::readData(const_raw_data buffer, raw_data output,
    size16 length)
{
    memcpy(output, buffer, length);
    return length;
}


size16 InputBinaryStream::readData(raw_data output, size16 length)
{
    InputBinaryStream::readData(buffer + offset, output, length);
    offset += length;
    return length;
}


template <>
size16 InputBinaryStream::read<float>(const_raw_data buffer, float* output)
{
    // TODO: poprawic...
    *output = *reinterpret_cast<const float*>(buffer);
    return sizeof(float);
}


}


