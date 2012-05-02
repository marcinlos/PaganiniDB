#include "config.h"
#include "paging/PageHeader.h"
#include "paging/Page.h"

namespace paganini
{

PageHeader::PageHeader(page_number number, PageType type)
{
    fill(number, type);
}


void PageHeader::fill(page_number number, PageType type)
{
    this->number = number;
    this->owner = owner;
    
    prev = next = NULL_PAGE;
    rows = 0;
    free_space = DATA_SIZE;
    free_offset = DATA_OFFSET;
    setPageType(flags, type);
}

        
}
