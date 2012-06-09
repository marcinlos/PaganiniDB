/*
    Definicje sterujace systemem stronnicowania.
*/
#ifndef __PAGANINI_PAGING_CONFIGURATION_H__
#define __PAGANINI_PAGING_CONFIGURATION_H__

#include <paganini/paging/types.h>
#include <paganini/paging/PageHeader.h>


namespace paganini
{


// Flagi

// Ilosc bitow na typ strony
static const int BITS_PER_PAGE_TYPE = 4;

// Maska typu strony
static const page_flags PAGE_TYPE_MASK = (1 << BITS_PER_PAGE_TYPE) - 1;

// Makro zwracajace typ strony z podanej flagi
inline PageType get_page_type(page_flags flags) 
{ 
    return static_cast<PageType>(flags & PAGE_TYPE_MASK); 
}

// Makro ustawiajace typ strony na podanej fladze
inline void set_page_type(page_flags& flags, PageType type)
{
    flags &= ~PAGE_TYPE_MASK;
    flags |= static_cast<page_flags>(type);
}

// Wielkosc strony jest stala
static const int PAGE_LOG_SIZE = 10;//13;
static const int PAGE_SIZE = (1 << PAGE_LOG_SIZE);

// Offset Row Offset Array (ROA) - jest na koncu strony, indeksy ida
// od tylu (i-ty indeks => page_addr + ROA - i * sizeof(pdbRowOffset)
static const page_offset ROA_OFFSET = PAGE_SIZE - sizeof(page_offset);

// Offset poczatku danych wierszy wzgledem poczatku strony
static const page_offset DATA_OFFSET = HEADER_SIZE;

// Wielkosc sekcji danych (wiersze + ROA) na stronie
static const size16 DATA_SIZE = PAGE_SIZE - DATA_OFFSET;

// Ilosc stron obslugiwanych przez jedna strone UV
static const size32 PAGES_PER_UV = 8;

}

#endif // __PAGANINI_PAGING_CONFIGURATION_H__

