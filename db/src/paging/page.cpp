#include "config.h"
#include "paging/page.h"

namespace paganini
{

void pdbFillHeader(pdbPageHeader* header, page_number number)
{
    header->number = number;
    header->next = header->prev = NULL_PAGE;
    header->owner = NULL_OBJ;
    header->rows = 0;
    header->free_space = DATA_SIZE;
    header->free_offset = DATA_OFFSET;
    setPageType(header->flags, PageType::UNUSED);
}

}
