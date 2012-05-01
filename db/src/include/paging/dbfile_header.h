/*
    Definicja naglowka pliku bazy danych, zapisanego w sekcji danych pierwszej
    strony pliku.
*/

#ifndef __PAGING_DBFILE_HEADER_H__
#define __PAGING_DBFILE_HEADER_H__

#ifdef _PAGANINI
#   include "paging/types.h"
#else 
#   include <paganini/paging/types.h>
#endif

#define MAX_NAME_LENGTH 256


typedef struct _pdbDatabaseHeader
{
    // Ilosc stron w pliku
    pdbSize32 page_count;
    
    // Nazwa bazy danych - ograniczonej dlugosci dla wygody
    char db_name[MAX_NAME_LENGTH];
    
    // Czas utworzenia bazy danych
    pdbTimestamp creation_time;
}
pdbDatabaseHeader;

#endif // __PAGING_DBFILE_HEADER_H__

