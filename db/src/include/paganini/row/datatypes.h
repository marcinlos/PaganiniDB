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
#include <paganini/util/format.h>
#include <string>
#include <cstring>
#include <iostream>
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
    // Stala oznaczajaca, ze wielkosc typu nie jest znana na etapie kompilacji.
    // Wciaz jednak moze byc stala (typ tekstowy)
    static const size16 VARIABLE_SIZE = -1;
    
    // Maska zmiennego rozmiaru do typow
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
    
    // Statyczne informacje o typach
    template <FieldType type>
    struct FieldTypeTraits;
    
    
    // Funkcja sprawdzajaca, czy typ jest typem zmiennej dlugosci
    inline bool is_variable_size(FieldType type)
    {
        return (static_cast<int>(type) & VARIABLE_MASK) != 0;
    }

    
    // Abstrakcyjna klasa bazowa dla typow danych
    struct Data
    {  
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
        
        // Do debugowania - wypisuje zawartosc w formacie human-readable
        virtual string toString() const = 0;
        
        virtual ~Data() { }
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
        
        string toString() const 
        { 
            return util::format("{}", val); 
        }
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
        
        explicit Char(size16 length, const string& val = ""): val(val), 
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
        
        string toString() const { return val; }
    };
    
    // Typ tekstowy bez ograniczenia dlugosci
    struct VarChar: public Data
    {
        string val;
        
        VarChar()
        {
        }
        
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
        
        size16 fieldSize() const { return VARIABLE_SIZE; }
        size16 size() const { return val.size(); }
        FieldType type() const { return FieldType::VarChar; }
        
        string toString() const { return val; }
    };
    
    
    // Opis pol
    
    template <>
    struct FieldTypeTraits<FieldType::Int>
    {
        static const size16 size = 4;
        typedef Int DefaultClass;   
    };
    
    template <>
    struct FieldTypeTraits<FieldType::PageNumber>
    {
        static const size16 size = sizeof(page_number);
        typedef PageNumber DefaultClass;   
    };
    
    template <>
    struct FieldTypeTraits<FieldType::Float>
    {
        static const size16 size = sizeof(float);
        typedef Float DefaultClass;
    };
    
    template <>
    struct FieldTypeTraits<FieldType::Char>
    {
        static const size16 size = VARIABLE_SIZE;
        typedef Char DefaultClass;
    };
    
    template <>
    struct FieldTypeTraits<FieldType::VarChar>
    {
        static const size16 size = VARIABLE_SIZE;
        typedef VarChar DefaultClass;
    };
}

}

// Fabryka potrzebuje hashowalnych typow
namespace std
{
    template <>
    struct hash<paganini::types::FieldType>
    {
        std::size_t operator () (paganini::types::FieldType type) const
        {
            return static_cast<std::size_t>(type);
        }
    };
}

#endif // __PAGANINI_ROW_DATATYPES_H__
