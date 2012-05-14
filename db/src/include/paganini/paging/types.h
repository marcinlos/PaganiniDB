/*
    Typy danych do wewnetrznego uzytku systemu stronnicowania. Korzystam z 
    stdint.h dla przenosnosci plikow bazy danych, choc nie zalatwia to innych
    spraw z tym zwiazanych, np. endianess.
*/

#ifndef __PAGANINI_PAGING_TYPES_H__
#define __PAGANINI_PAGING_TYPES_H__

#include <cstdint>

namespace paganini
{

// Numer strony w pliku
typedef int32_t page_number;

// Identyfikator obiektu w bazie danych
typedef int32_t object_id;

// Offset wewnatrz strony
typedef int16_t page_offset;

// Numer wiersza w tabeli
typedef int16_t row_number;

// Numer kolumny w wierszu
typedef int16_t column_number;

// 16-bitowy typ oznaczajacy wielkosc
typedef uint16_t size16;

// 32-bitowy typ oznaczajacy wielkosc
typedef uint32_t size32;

// Typ na dodatkowe flagi dla strony
typedef uint16_t page_flags;

// Typ na dodatkowe flagi dla wiersza
typedef uint16_t row_flags;

// Typ na dodatkowe flagi dla kolumny
typedef uint16_t column_flags;



// Czas UNIXowy
typedef int64_t timestamp;

// Typedefy do surowych danych
typedef char* raw_data;
typedef const char* const_raw_data;


// Definicje nieprawidlowych (pustych) odniesien

// Pusty wskaznik strony
static const page_number NULL_PAGE = -1;

// Pusty numer wiersza
static const page_offset NULL_ROW = -1;

// Pusty numer obiektu
static const object_id NULL_OBJ = -1;

// Numer nieistniejacej kolumny
static const column_number NULL_COLUMN = -1;

}

#endif // __PAGANINI_PAGING_TYPES_H__ 

