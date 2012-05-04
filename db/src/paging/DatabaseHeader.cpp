#include "config.h"
#include <paganini/paging/DatabaseHeader.h>
#include <ctime>
#include <cstring>

namespace paganini
{

DatabaseHeader::DatabaseHeader(const string& name, size32 page_count)
{
    creation_time = time(nullptr);
    this->page_count = page_count;
    strncpy(db_name, name.c_str(), MAX_NAME_LENGTH);
}

}
