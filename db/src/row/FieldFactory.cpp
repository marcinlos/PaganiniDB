#include "config.h"
#include <paganini/row/FieldFactory.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{


FieldFactory::FieldFactory()
{    
    using namespace types;
    
    types[ContentType::Int] =
    {
        [](size16) { return new Int; },
        [](size16) { return sizeof(int); }
    };
    
    types[ContentType::PageNumber] =
    {
        [](size16) { return new PageNumber; },
        [](size16) { return sizeof(page_number); }
    };    
    
    types[ContentType::Float] =
    {
        [](size16) { return new Float; },
        [](size16) { return sizeof(float); }
    };
    
    types[ContentType::Char] =
    {
        [](size16 size) { return new Char(size); },
        [](size16 size) { return size; }
    };
    
    types[ContentType::VarChar] =
    {
        [](size16) { return new VarChar; },
        [](size16) { return VARIABLE_SIZE; }
    };
}


FieldFactory::DataPtr FieldFactory::create(types::FieldType type) const
{
    auto i = types.find(type.content);
    if (i == types.end())
        throw std::logic_error(util::format("Missing creator for '{}'"
            "field type", type));
            
    return DataPtr(i->second.creator(type.size));
}

size16 FieldFactory::size(types::FieldType type) const
{
    auto i = types.find(type.content);
    if (i == types.end())
        throw std::logic_error(util::format("Missing creator for '{}'"
            "field type", type));
            
    return i->second.size(type.size);
}

/*
const FieldFactory::Metadata& 
    FieldFactory::operator [](types::FieldType type) const
{
    return getMetadata(type);
}

const FieldFactory::Metadata& 
    FieldFactory::getMetadata(types::FieldType type) const
{
    auto i = types.find(type);
    if (i == types.end())
        throw std::logic_error(util::format("Missing entry for '{}'"
            "field type", static_cast<int>(type)));
            
    return i->second;
}
*/

FieldFactory::Creator FieldFactory::registerCreator(types::ContentType content, 
    Creator creator)
{
    auto i = types.find(content);
    if (i != types.end())
    {
        FieldMetadata& d = i->second;
        Creator previous = d.creator;
        d.creator = creator;
        return previous;
    }
    throw std::logic_error(util::format("Missing entry for '{}'"
        "field type", content));
}


}

