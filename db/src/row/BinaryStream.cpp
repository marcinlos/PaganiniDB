#include "config.h"
#include <paganini/row/BinaryStream.h>

namespace paganini
{


BinaryStream::BinaryStream(raw_data buffer, size16 offset): 
    buffer(buffer), offset(offset)
{
}


page_offset BinaryStream::getOffset() const
{
    return offset;
}

void BinaryStream::setOffset(page_offset offset)
{
    this->offset = offset;
}


void BinaryStream::skip(page_offset offset)
{
    this->offset += offset;
}


raw_data BinaryStream::getBuffer()
{
    return buffer + offset;
}


}


