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

// Rodzaje stron
enum class PageType// : page_flags
{
    UNUSED = 15,    // Nieuzywana strona (domyslnie)
    HEADER = 1,     // pierwsza strona bazy danych
    DATA = 2,       // strona z danymi
    UV = 3          // Usage Vector
};

struct PageHeader
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
    
    // Wypelnia informacje naglowka (dziala tak samo, jak konstruktor)
    void fill(page_number number, PageType type = PageType::UNUSED);
    
    PageHeader(page_number number, PageType type = PageType::UNUSED);
}; 

static const int HEADER_SIZE = sizeof(PageHeader);

}


#endif // __PAGING_PAGE_HEADER_H__

