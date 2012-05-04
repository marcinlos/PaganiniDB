/*
    Definicje narzedzi do obslugi bledow: typ wyliczeniowy bledu, oraz
    klasa wyjatku bazy danych.
*/
#ifndef __PAGANINI_ERROR_H__
#define __PAGANINI_ERROR_H__

#include <functional>
#include <stdexcept>
#include <string>
using std::string;


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
    ,DATA_TOO_LONG      // Podane dane nie mieszcza sie w typie
};


// Funkcja zwracajaca tekstowy opis bledu
const char* get_error_message(Error code);


// Bazowa klasa wyjatkow bazy danych
class Exception: public std::runtime_error
{
private:
    Error error;
    
public:
    explicit Exception(Error code): std::runtime_error("?"), error(code)
    {
    }
    
    Exception(const string& message, Error code): 
        std::runtime_error(message), error(code)
    {
    }

    // Zwraca kod bledu
    Error getCode() const
    {
        return error;
    }
    
    const char* getCodeMessage() const
    {
        return get_error_message(error);
    }
};

}

// Definicja hasha dla enuma Error. Czemu to jest potrzebne ?! Buu!
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

#endif // __PAGANINI_ERROR_H__
