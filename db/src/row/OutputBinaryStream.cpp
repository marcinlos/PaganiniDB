#include "config.h"
#include <paganini/row/OutputBinaryStream.h>

namespace paganini
{


OutputBinaryStream::OutputBinaryStream(raw_data buffer, size16 offset): 
    BinaryStream(buffer, offset)
{
}


size16 OutputBinaryStream::writeData(raw_data buffer, 
    const_raw_data data, size16 length)
{
    memcpy(buffer, data, length);
    return length;
}


template <>
size16 OutputBinaryStream::write<float>(raw_data buffer, const float& value)
{
    // TODO: poprawic...
    *reinterpret_cast<float*>(buffer) = value;
    return sizeof(value);
}

/*
template <>
size16 OutputBinaryStream::write<double>(raw_data buffer, const double& value)
{
    // TODO: poprawic...
    *reinterpret_cast<float*>(buffer) = value;
    return sizeof(value);
}*/


size16 OutputBinaryStream::writeData(const_raw_data data, size16 length)
{
    OutputBinaryStream::writeData(buffer + offset, data, length);
    offset += length;
    return length;
}


}


