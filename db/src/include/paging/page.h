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

// Rodzaje stron
#define UNUSED_PAGE ((pdbPageType) 15)  // Nieuzywana strona (domyslnie)
#define HEADER_PAGE ((pdbPageType) 0)   // pierwsza strona bazy danych
#define DATA_PAGE   ((pdbPageType) 1)   // strona z danymi
#define UV_PAGE     ((pdbPageType) 2)   // Usage Vector

// Specjalne strony
#define HEADER_PAGE_NUMBER      0
#define FIRST_UV_PAGE_NUMBER    1

// Flagi

// Ilosc bitow na typ strony
#define BITS_PER_PAGE_TYPE 4

// Maska typu strony
#define PAGE_TYPE_MASK ((pdbPageFlags) ((1 << BITS_PER_PAGE_TYPE) - 1))

// Makro zwracajace typ strony z podanej flagi
#define PAGE_TYPE(flags) ((pdbPageType) (flags & PAGE_TYPE_MASK))

// Makro ustawiajace typ strony na podanej fladze
#define SET_PAGE_TYPE(flags, type) ((flags &= (~ PAGE_TYPE_MASK)), flags |= type)

// Wielkosc strony jest stala
#define PAGE_LOG_SIZE 13
#define PAGE_SIZE (1 << PAGE_LOG_SIZE)

// Offset Row Offset Array (ROA) - jest na koncu strony, indeksy ida
// od tylu (i-ty indeks => page_addr + ROA - i * sizeof(pdbRowOffset)
#define ROA_OFFSET (PAGE_SIZE - sizeof(pdbRowOffset))

// Offset poczatku danych wierszy wzgledem poczatku strony
#define DATA_OFFSET HEADER_SIZE

// Wielkosc sekcji danych (wiersze + ROA) na stronie
#define DATA_SIZE (PAGE_SIZE - DATA_OFFSET)

// Ilosc stron obslugiwanych przez jedna strone UV
#define PAGES_PER_UV 8000

// Sprawdza, czy strona o podanym numerze to strona UV
#define IS_UV(page_number) ((page_number - 1) % (PAGES_PER_UV + 1) == 0)


// Typedefy do surowych danych
typedef unsigned char* pdbData;
typedef unsigned char pdbPageBuffer[PAGE_SIZE]; 

// Wypelnia naglowek domyslnymi danymi
void pdbFillHeader(pdbPageHeader* header, pdbPageNumber number);



#endif // __PAGING_PAGE_H__

