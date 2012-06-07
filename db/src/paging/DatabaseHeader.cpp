#include "config.h"
#include <paganini/paging/DatabaseHeader.h>
#include <ctime>

namespace paganini
{

DatabaseHeader::DatabaseHeader(const string& name, size32 page_count)
{
    creation_time = time(nullptr);
    this->page_count = page_count;
    name.copy(db_name, MAX_NAME_LENGTH);
}

}
