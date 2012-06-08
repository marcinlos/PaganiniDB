#include "config.h"
#include <paganini/indexing/ComparatorFactory.h>

namespace paganini
{


ComparatorFactory::ComparatorFactory()
{
    using namespace types;
    
    comparators_[ContentType::Int] = ContainedPointer(
        new DataComparator<ContentType::Int, CastingPolicy>);
    
    comparators_[ContentType::PageNumber] = ContainedPointer(
        new DataComparator<ContentType::PageNumber, CastingPolicy>);
    
    comparators_[ContentType::Float] = ContainedPointer(
        new DataComparator<ContentType::Float, CastingPolicy>);
    
    comparators_[ContentType::Char] = ContainedPointer(
        new DataComparator<ContentType::Char, CastingPolicy>);
    
    comparators_[ContentType::VarChar] = ContainedPointer(
        new DataComparator<ContentType::VarChar, CastingPolicy>);
}


ComparatorFactory::ComparatorPointer ComparatorFactory::get(
    types::ContentType type) const
{
    auto i = comparators_.find(type);
    if (i == comparators_.end())
        throw std::logic_error(util::format("Missing comparator for '{}'"
            "field type", type));
            
    return i->second.get();
}


}
