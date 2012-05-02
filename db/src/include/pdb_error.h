#ifndef __PDB_ERROR_H__
#define __PDB_ERROR_H__
#include <functional>


namespace paganini
{

// Wartosci kodow bledu
enum class Error
{
     NONE               // Brak bledu
    ,ARMAGEDON          // Test
    ,FILECREATE         // Nie udalo sie utworzyc pliku bazy danych
    ,FILEOPEN           // Nie udalo sie otworzyc pliku
    ,WRITE              // Blad zapisu
    ,READ               // Blad odczytu
    ,SEEK               // Blad pozycjonowania w pliku
};


// Funkcja zwracajaca tekstowy opis bledu
const char* pdbErrorMsg(Error code);

// Funkcja pobierajaca kod bledu
Error pdbErrno();

// Funkcja ustawiajaca kod bledu - do uzytku wewnetrznego
int _pdbSetErrno(Error code);


}

// Definicja hasha dla enuma Error. Czemu to jest potrzebne ?!
namespace std
{
    template <>
    struct hash<paganini::Error>
    {
        std::size_t operator () (paganini::Error code) const
        {
            return static_cast<std::size_t>(code);
        }
    };
}

#endif // __PDB_ERROR_H__
