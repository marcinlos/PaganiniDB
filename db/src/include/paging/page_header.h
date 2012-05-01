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


typedef struct _pdbPageHeader
{
    // Numer strony w pliku
    pdbPageNumber page_number;
    
    // Numery stron: kolejnej i poprzedniej w przypadku, gdy konieczne
    // jest laczenie ich w liste
    pdbPageNumber next;
    pdbPageNumber prev;    
    
    // Obiekt bazy danych bedacy wlascicielem strony
    pdbObjectId owner;
    
    // Ilosc wierszy przechowywanych na stronie
    pdbRowCount rows;
    
    // Ilosc wolnego miejsca na stronie
    pdbSize16 free_space;
    
    // Poczatek wolnego miejsca na stronie
    pdbRowOffset free_offset;
    
    // Flagi strony (BITS_PER_PAGE_TYPE najmlodszych bitow to typ strony)
    pdbPageFlags flags;
} 
pdbPageHeader;


#define HEADER_SIZE (sizeof(pdbPageHeader))

#endif // __PAGING_PAGE_HEADER_H__

