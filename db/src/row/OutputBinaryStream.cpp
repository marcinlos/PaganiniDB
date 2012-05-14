#include "config.h"
#include <paganini/row/OutputBinaryStream.h>

namespace paganini
{


OutputBinaryStream::OutputBinaryStream(raw_data buffer, size16 offset): 
    buffer(buffer), offset(offset)
{
}


size16 OutputBinaryStream::write(raw_data buffer, 
    const_raw_data data, size16 length)
{
    memcpy(buffer, data, length);
    return length;
}


template <>
size16 OutputBinaryStream::write<float>(raw_data buffer, float value)
{
    // TODO: poprawic...
    *reinterpret_cast<float*>(buffer) = value;
    return sizeof(value);
}


size16 OutputBinaryStream::write(const_raw_data data, size16 length)
{
    OutputBinaryStream::write(buffer + offset, data, length);
    offset += length;
    return length;
}


page_offset OutputBinaryStream::getOffset() const
{
    return offset;
}


void OutputBinaryStream::skip(size16 offset)
{
    this->offset += offset;
}


raw_data OutputBinaryStream::getBuffer()
{
    return buffer + offset;
}


}


