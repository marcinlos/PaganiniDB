#include "config.h"
#include <paganini/paging/PageHeader.h>
#include <paganini/paging/configuration.h>

namespace paganini
{


PageHeader::PageHeader(page_number number, PageType type)
{
    fill(number, type);
}


void PageHeader::fill(page_number number, PageType type)
{
    this->number = number;
    this->owner = NULL_OBJ;
    
    prev = next = NULL_PAGE;
    rows = 0;
    free_space = DATA_SIZE;
    free_offset = 0; // od poczatku sekcji danych
    flags = 0;
    set_page_type(flags, type);
}

        
}
