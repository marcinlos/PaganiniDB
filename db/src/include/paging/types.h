/*
    Typy danych do wewnetrznego uzytku systemu stronnicowania. Korzystam z 
    stdint.h dla przenosnosci plikow bazy danych, choc nie zalatwia to innych
    spraw z tym zwiazanych, np. endianess.
*/

#ifndef __PAGING_TYPES_H__
#define __PAGING_TYPES_H__

#include <cstdint>

namespace paganini
{

// Numer strony w pliku
typedef int32_t page_number;

// Identyfikator obiektu w bazie danych
typedef int32_t object_id;

// Offset wiersza wewnatrz strony
typedef int16_t row_offset;

// Ilosc wierszy
typedef int16_t row_count;

// 16-bitowy typ oznaczajacy wielkosc
typedef uint16_t size16;

// 32-bitowy typ oznaczajacy wielkosc
typedef uint32_t size32;

// Typ na dodatkowe flagi dla strony
typedef uint16_t page_flags;

// Czas UNIXowy
typedef int64_t timestamp;


// Definicje nieprawidlowych (pustych) odniesien

// Pusty wskaznik strony
static const page_number NULL_PAGE = -1;

// Pusty numer wiersza
static const row_offset NULL_ROW = -1;

// Pusty numer obiektu
static const object_id NULL_OBJ = -1;

}

#endif // __PAGING_TYPES_H__ 

