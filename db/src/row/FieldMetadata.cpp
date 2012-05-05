#include "config.h"
#include <paganini/row/FieldMetadata.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{


FieldMetadata::FieldMetadata()
{    
    types[types::FieldType::Int] = Metadata
    {
        [](size16) 
        { 
            return new 
                typename types::FieldTypeTraits<
                    types::FieldType::Int
                >::DefaultClass; 
        },
        types::FieldTypeTraits<
            types::FieldType::Int
        >::size
    };
    
    types[types::FieldType::PageNumber] = Metadata
    {
        [](size16) 
        { 
            return new 
                typename types::FieldTypeTraits<
                    types::FieldType::PageNumber
                >::DefaultClass; 
        },
        types::FieldTypeTraits<
            types::FieldType::PageNumber
        >::size
    };    
    
    types[types::FieldType::Float] = Metadata
    {
        [](size16) 
        { 
            return new 
                typename types::FieldTypeTraits<
                    types::FieldType::Float
                >::DefaultClass; 
        },
        types::FieldTypeTraits<
            types::FieldType::Float
        >::size
    };
    
    types[types::FieldType::Char] = Metadata
    {
        [](size16 size) 
        { 
            return new 
                typename types::FieldTypeTraits<
                    types::FieldType::Char
                >::DefaultClass(size); 
        },
        types::FieldTypeTraits<
            types::FieldType::Char
        >::size
    };
    
    types[types::FieldType::VarChar] = Metadata
    {
        [](size16) 
        { 
            return new 
                typename types::FieldTypeTraits<
                    types::FieldType::VarChar
                >::DefaultClass; 
        },
        types::FieldTypeTraits<
            types::FieldType::VarChar
        >::size
    };
}


FieldMetadata::DataPtr FieldMetadata::create(types::FieldType type, 
    size16 size) const
{
    auto i = types.find(type);
    if (i == types.end())
        throw std::logic_error(util::format("Missing creator for '{}'"
            "field type", static_cast<int>(type)));
            
    return DataPtr(std::get<CREATOR>((i->second))(size));
}


size16 FieldMetadata::getSize(types::FieldType type) const
{
    auto i = types.find(type);
    if (i == types.end())
        throw std::logic_error(util::format("Missing entry for '{}'"
            "field type", static_cast<int>(type)));
            
    return std::get<SIZE>(i->second);
}


FieldMetadata::Creator FieldMetadata::registerCreator(types::FieldType type, 
    Creator creator)
{
    auto i = types.find(type);
    if (i != types.end())
    {
        Metadata& d = i->second;
        Creator previous = std::get<0>(d);
        std::get<CREATOR>(d) = creator;
        return previous;
    }
    throw std::logic_error(util::format("Missing entry for '{}'"
        "field type", static_cast<int>(type)));
}


}

