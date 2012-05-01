/*
    Typy danych do wewnetrznego uzytku systemu stronnicowania. Korzystam z 
    stdint.h dla przenosnosci plikow bazy danych, choc nie zalatwia to innych
    spraw z tym zwiazanych, np. endianess.
*/

#ifndef __PAGING_TYPES_H__
#define __PAGING_TYPES_H__

#include <stdint.h>


// Numer strony w pliku
typedef int32_t pdbPageNumber;

// Identyfikator obiektu w bazie danych
typedef int32_t pdbObjectId;

// Offset wiersza wewnatrz strony
typedef int16_t pdbRowOffset;

// Ilosc wierszy
typedef int16_t pdbRowCount;

// 16-bitowy typ oznaczajacy wielkosc
typedef uint16_t pdbSize16;

// 32-bitowy typ oznaczajacy wielkosc
typedef uint32_t pdbSize32;

// Typ strony
typedef uint8_t pdbPageType;

// Typ na dodatkowe flagi dla strony
typedef uint16_t pdbPageFlags;

// Czas UNIXowy
typedef int64_t pdbTimestamp;


// Definicje nieprawidlowych (pustych) odniesien

// Pusty wskaznik strony
#define NULL_PAGE ((pdbPageNumber) -1)

// Pusty numer wiersza
#define NULL_ROW ((pdbRowOffset) -1)

// Pusty numer obiektu
#define NULL_OBJ ((pdbObjectId) -1)


#endif // __PAGING_TYPES_H__ 

