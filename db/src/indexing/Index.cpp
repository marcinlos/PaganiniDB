#include "config.h"
#include <paganini/indexing/Index.h>

namespace paganini
{


Index::Index(const types::FieldType& type, ConstDataPtr value, 
    page_number child):
    type_(type), value_(value), child_(child)
{
}


Index::Index(const Index& other): type_(other.type_), value_(other.value_),
    child_(other.child_)
{
}


Index& Index::operator = (const Index& other)
{
    type_ = other.type_;
    value_ = other.value_;
    child_ = other.child_;
    return *this;
}


}
