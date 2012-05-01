/*
    Definicje flag/makr dotyczacych stron.
*/
#ifndef __PAGING_PAGE_H__
#define __PAGING_PAGE_H__

#ifdef _PAGANINI
#   include "paging/page_header.h"
#   include "paging/types.h"
#else 
#   include <paganini/paging/page_header.h>
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


// Specjalne strony
static const page_number HEADER_PAGE_NUMBER = 0;
static const page_number FIRST_UV_PAGE_NUMBER = 1;

// Flagi

// Ilosc bitow na typ strony
static const int BITS_PER_PAGE_TYPE = 4;

// Maska typu strony
static const page_flags PAGE_TYPE_MASK = (1 << BITS_PER_PAGE_TYPE) - 1;

// Makro zwracajace typ strony z podanej flagi
inline PageType getPageType(page_flags flags) 
{ 
    return static_cast<PageType>(flags & PAGE_TYPE_MASK); 
}

// Makro ustawiajace typ strony na podanej fladze
inline void setPageType(page_flags& flags, PageType type)
{
    flags &= ~PAGE_TYPE_MASK;
    flags |= static_cast<page_flags>(type);
}

// Wielkosc strony jest stala
static const int PAGE_LOG_SIZE = 13;
static const int PAGE_SIZE = (1 << PAGE_LOG_SIZE);

// Offset Row Offset Array (ROA) - jest na koncu strony, indeksy ida
// od tylu (i-ty indeks => page_addr + ROA - i * sizeof(pdbRowOffset)
static const row_offset ROA_OFFSET = PAGE_SIZE - sizeof(row_offset);

// Offset poczatku danych wierszy wzgledem poczatku strony
static const row_offset DATA_OFFSET = HEADER_SIZE;

// Wielkosc sekcji danych (wiersze + ROA) na stronie
static const size16 DATA_SIZE = PAGE_SIZE - DATA_OFFSET;

// Ilosc stron obslugiwanych przez jedna strone UV
static const size32 PAGES_PER_UV = 8000;

// Sprawdza, czy strona o podanym numerze to strona UV
inline bool isUV(page_number page)
{
    return (page - 1) % (PAGES_PER_UV + 1) == 0;
}


// Typedefy do surowych danych
typedef unsigned char* pdbData;
typedef unsigned char pdbPageBuffer[PAGE_SIZE]; 

// Wypelnia naglowek domyslnymi danymi
void pdbFillHeader(pdbPageHeader* header, page_number number);

}

#endif // __PAGING_PAGE_H__

