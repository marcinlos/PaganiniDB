/*
    Definicja naglowka strony.
*/

#ifndef __PAGING_PAGE_HEADER_H__
#define __PAGING_PAGE_HEADER_H__

#ifdef _PAGANINI
#   include "paging/types.h"
#else 
#   include <paganini/paging/types.h>
#endif

namespace paganini
{

struct pdbPageHeader
{
    // Numer strony w pliku
    page_number number;
    
    // Numery stron: kolejnej i poprzedniej w przypadku, gdy konieczne
    // jest laczenie ich w liste
    page_number next;
    page_number prev;    
    
    // Obiekt bazy danych bedacy wlascicielem strony
    object_id owner;
    
    // Ilosc wierszy przechowywanych na stronie
    row_count rows;
    
    // Ilosc wolnego miejsca na stronie
    size16 free_space;
    
    // Poczatek wolnego miejsca na stronie
    row_offset free_offset;
    
    // Flagi strony (BITS_PER_PAGE_TYPE najmlodszych bitow to typ strony)
    page_flags flags;
}; 

}


#define HEADER_SIZE (sizeof(pdbPageHeader))

#endif // __PAGING_PAGE_HEADER_H__

