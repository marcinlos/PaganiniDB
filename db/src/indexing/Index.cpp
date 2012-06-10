#include "config.h"
#include <paganini/indexing/Index.h>

namespace paganini
{


Index::Index(const types::FieldType& type, ConstDataPtr value, 
    page_number child):
    type_(type), value_(value), child_(child)
{
}


}
