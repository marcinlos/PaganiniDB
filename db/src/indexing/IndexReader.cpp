#include "config.h"
#include <paganini/indexing/IndexReader.h>
#include <paganini/io/InputBinaryStream.h>

namespace paganini
{


IndexReader::IndexReader(): factory(FieldFactory::getInstance())
{
}


IndexReader::ReturnType IndexReader::read(raw_data buffer, 
    types::FieldType format)
{
    typedef std::unique_ptr<types::Data> DataPtr;     
    InputBinaryStream stream(buffer);
    
    page_number child;
    stream.read(&child);
    
    size16 size = factory.size(format);
    if (size == types::VARIABLE_SIZE)
        stream.read(&size);
        
    DataPtr value = factory.create(format);
    value->readFrom(stream, size);
    return ReturnType(new Index(format, 
        Index::ConstDataPtr(value.release()), child));
}


}
