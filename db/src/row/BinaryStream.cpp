#include "config.h"
#include <paganini/row/BinaryStream.h>

namespace paganini
{


BinaryStream::BinaryStream(raw_data buffer, size16 offset): 
    buffer_(buffer), offset_(offset)
{
}


page_offset BinaryStream::getOffset() const
{
    return offset_;
}

void BinaryStream::setOffset(page_offset offset)
{
    offset_ = offset;
}


void BinaryStream::skip(page_offset offset)
{
    offset_ += offset;
}


raw_data BinaryStream::getBuffer()
{
    return buffer_ + offset_;
}


}


