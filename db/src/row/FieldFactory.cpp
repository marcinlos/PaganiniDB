#include "config.h"
#include <paganini/row/FieldFactory.h>
#include <paganini/util/format.h>
#include <paganini/row/TypeMapping.h>
#include <stdexcept>


namespace paganini
{


FieldFactory::FieldFactory()
{    
    using namespace types;
    
    types[ContentType::Int] =
    {
        [](size16) { return new types::TypeMapping<ContentType::Int>::type; },
        [](size16) { return sizeof(int); }
    };
    
    types[ContentType::PageNumber] =
    {
        [](size16) { return new 
            types::TypeMapping<ContentType::PageNumber>::type; },
        [](size16) { return sizeof(page_number); }
    };    
    
    types[ContentType::Float] =
    {
        [](size16) { return new types::TypeMapping<ContentType::Float>::type; },
        [](size16) { return sizeof(float); }
    };
    
    types[ContentType::Char] =
    {
        [](size16 size) { return new 
            types::TypeMapping<ContentType::Char>::type(size); },
        [](size16 size) { return size; }
    };
    
    types[ContentType::VarChar] =
    {
        [](size16) { return new types::TypeMapping<ContentType::VarChar>::type; },
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

