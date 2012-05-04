/*
    Definicja naglowka pliku bazy danych, zapisanego w sekcji danych pierwszej
    strony pliku.
*/

#ifndef __PAGANINI_PAGING_DATABASE_HEADER_H__
#define __PAGANINI_PAGING_DATABASE_HEADER_H__

#ifdef _PAGANINI
#   include "paging/types.h"
#else 
#   include <paganini/paging/types.h>
#endif
#include <string>
using std::string;

namespace paganini
{

struct DatabaseHeader
{
    // Maksymalna dlugosc nazwy bazy danych
    static const int MAX_NAME_LENGTH = 256;
    
    // Ilosc stron w pliku
    size32 page_count;
    
    // Nazwa bazy danych - ograniczonej dlugosci dla wygody
    char db_name[MAX_NAME_LENGTH];
    
    // Czas utworzenia bazy danych
    timestamp creation_time;
    
    // Wypelnia naglowek odpowiednimi wartosciami
    explicit DatabaseHeader(const string& name, size32 page_count = 1);
};

}

#endif // __PAGANINI_PAGING_DATABASE_HEADER_H__

