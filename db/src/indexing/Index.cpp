#include "config.h"
#include <paganini/indexing/Index.h>

namespace paganini
{


Index::Index(types::FieldType type, types::Data* value, page_number child):
    type_(type), value_(value), child_(child)
{
}


}
