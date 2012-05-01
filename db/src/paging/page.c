#include "config.h"
#include "paging/page.h"

void pdbFillHeader(pdbPageHeader* header, pdbPageNumber number)
{
    header->page_number = number;
    header->next = header->prev = NULL_PAGE;
    header->owner = NULL_OBJ;
    header->rows = 0;
    header->free_space = DATA_SIZE;
    header->free_offset = DATA_OFFSET;
    header->flags = UNUSED_PAGE;
}
