/*
    Definicje zwiazane z reprezentacja bazodanowych typow danych w systemie.
    Zdefiniowany jest tu typ wyliczeniowy okreslajacy rodzaj danej, oraz
    klasa bazowa dla typow je reprezentujacych. Naglowek zawiera rowniez
    szablon implementacji dla typow POD, oraz definicje klas dla typow
    tekstowych.
*/
#ifndef __PAGANINI_ROW_DATATYPES_H__
#define __PAGANINI_ROW_DATATYPES_H__

#include <paganini/paging/Page.h>
#include <paganini/paging/types.h>
#include <string>
#include <cstring>
using std::string;

namespace paganini
{

// Numer kolumny w wierszu
typedef int16_t column_number;

// Flagi kolumny
typedef uint16_t column_flags;

// Numer nieistniejacej kolumny
static const column_number NULL_COLUMN = -1;


namespace types
{
    // Maska zmiennego rozmiaru
    static const int VARIABLE_MASK = 1 << 7;
    
    // Dostepne rodzaje pol... Troche to redundancji wprowadza, ale
    // nie widze mozliwosci statycznego unikniecia tego niestety.
    // Brzydka decyzja projektowa: osmy bit = 1 oznacza zmienny rozmiar.
    enum class FieldType
    {
        None,
        Int,
        Float,
        PageNumber,
        Char,
        // Odtad typy zmiennej wielkosci
        VarChar = VARIABLE_MASK
    };
    
    // Funkcja sprawdzajaca, czy typ jest typem zmiennej dlugosci
    inline bool is_variable_size(FieldType type)
    {
        return (static_cast<int>(type) & VARIABLE_MASK) != 0;
    }

    
    // Abstrakcyjna klasa bazowa dla typow danych
    struct Data
    {
        static const size16 VARIABLE = -1;
        
        // Zapisuje dane do bufora. Zwraca ilosc zapisanych bajtow
        virtual size16 writeTo(page_data buffer) const = 0;
        
        // Wczytuje dane z podanego zakresu bufora.
        virtual void readFrom(page_data begin, page_data end) = 0;
        
        // Zwraca rozmiar typu danych - nie konkretnej instancji. 
        // Zwraca VARIABLE, jesli wielkosc nie jest stala.
        virtual size16 fieldSize() const = 0;
        
        // Zwraca rozmiar tej konkretnej instancji typu danych.
        virtual size16 size() const = 0; 
        
        // Zwraca mapowany typ pola
        virtual FieldType type() const = 0;
    };
    
    
    // Powinno dzialac dla wszystkich POD-ow.
    template <typename U, FieldType V>
    struct GenericWrapper: public Data
    {
        typedef U Value;
        Value val;
        
        explicit GenericWrapper(Value val = Value()): val(val)
        {
        }
        
        size16 writeTo(page_data buffer) const
        {
            Value* b = reinterpret_cast<Value*>(buffer);
            *b = val;
        }
        
        void readFrom(page_data begin, page_data end)
        {
            val = *(reinterpret_cast<const Value*>(begin));
        }
        
        size16 fieldSize() const { return sizeof(val); }
        size16 size() const { return sizeof(val); }
        FieldType type() const { return V; }
    };
    
    // Dla podstawowych typow numerycznych wystarczy
    typedef GenericWrapper<int32_t, FieldType::Int> Int;
    typedef GenericWrapper<float, FieldType::Float> Float;
    typedef GenericWrapper<page_number, FieldType::PageNumber> PageNumber;
    
    
    // Stringami musimy zajac sie inaczej.
    // Typ tekstowy okreslonej z gory dlugosci.
    struct Char: public Data
    {
        string val;
        size16 max_length;
        
        explicit Char(size16 length): max_length(length) 
        {
        }
        
        Char(size16 length, const string& val): val(val), 
            max_length(length)
        {
        }
        
        size16 writeTo(page_data buffer) const
        {
            memset(buffer, 0xAB, max_length);
            strncpy(reinterpret_cast<char*>(buffer), val.c_str(), max_length);
        }
        
        void readFrom(page_data begin, page_data end)
        {
            val = string(begin, end);
        }
        
        size16 fieldSize() const { return max_length; }
        size16 size() const { return max_length; }
        FieldType type() const { return FieldType::Char; }
    };
    
    // Typ tekstowy bez ograniczenia dlugosci (w rozsadnych granicach :-)
    struct VarChar: public Data
    {
        string val;
        
        VarChar(const string& val): val(val)
        {
        }
        
        size16 writeTo(page_data buffer) const
        {
            strcpy(reinterpret_cast<char*>(buffer), val.c_str());
        }
        
        void readFrom(page_data begin, page_data end)
        {
            val = string(begin, end);
        }
        
        size16 fieldSize() const { return VARIABLE; }
        size16 size() const { return val.size(); }
        FieldType type() const { return FieldType::VarChar; }
    };
};

}

#endif // __PAGANINI_ROW_DATATYPES_H__
