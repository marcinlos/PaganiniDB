/*
    Definicja naglowka strony
*/
#ifndef __PAGANINI_PAGING_PAGE_HEADER_H__
#define __PAGANINI_PAGING_PAGE_HEADER_H__

#include <paganini/paging/types.h>
#include <iostream>


namespace paganini
{

// Rodzaje stron
enum class PageType// : page_flags
{
    UNUSED = 15,    // Nieuzywana strona (domyslnie)
    HEADER = 1,     // pierwsza strona bazy danych
    INDEX = 2,      // strona z indeksami
    DATA = 3,       // Strona z danymi
    UV = 4          // Usage Vector
};


inline std::ostream& operator << (std::ostream& os, PageType type)
{
    const char* str;
    switch (type)
    {
    case PageType::UNUSED: str = "unused"; break;
    case PageType::HEADER: str = "header"; break;
    case PageType::INDEX: str = "index"; break;
    case PageType::DATA: str = "data"; break;
    case PageType::UV: str = "usage vector"; break;
    default: str = "INVALID"; break;
    }
    return os << str;
}


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
    size16 rows;
    
    // Ilosc wolnego miejsca na stronie
    size16 free_space;
    
    // Poczatek wolnego miejsca na stronie (liczona od poczatku sekcji danych)
    page_offset free_offset;
    
    // Flagi strony (BITS_PER_PAGE_TYPE najmlodszych bitow to typ strony)
    page_flags flags;
    
    // Tworzy strone z podanym numerem i typem
    explicit PageHeader(page_number number, PageType type = PageType::UNUSED);
    
    // Wypelnia informacje naglowka (dziala tak samo, jak konstruktor)
    void fill(page_number number, PageType type = PageType::UNUSED);
}; 

static const int HEADER_SIZE = sizeof(PageHeader);

}


#endif // __PAGANINI_PAGING_PAGE_HEADER_H__

