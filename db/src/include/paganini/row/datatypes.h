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
#include <paganini/io/OutputBinaryStream.h>
#include <paganini/io/InputBinaryStream.h>
#include <paganini/paging/types.h>
#include <paganini/util/format.h>
#include <string>
#include <cstring>
#include <vector>
#include <functional>
#include <iostream>
using std::string;


namespace paganini
{

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
enum class ContentType
{
    None,
    Int,
    Float,
    PageNumber,
    Char,
    // Odtad typy zmiennej wielkosci
    VarChar = VARIABLE_MASK
};

inline std::ostream& operator << (std::ostream& os, ContentType content)
{
    const char* str;
    switch (content)
    {
    case ContentType::None: str = "none"; break;
    case ContentType::Int: str = "int"; break;
    case ContentType::Float: str = "float"; break;
    case ContentType::PageNumber: str = "page_number"; break;
    case ContentType::Char: str = "char"; break;
    case ContentType::VarChar: str = "varchar"; break;
    default: "INVALID";
    }
    return os << str;
}

// Pelny opis typu danych - rodzaj + wielkosc (dla char).
struct FieldType
{
    static const size16 NON_APPLICABLE = 0;
    ContentType content;
    size16 size;
    
    FieldType(ContentType content, size16 size = NON_APPLICABLE): 
        content(content), size(size)
    {
    }
};

// Operatory do FieldType
inline bool operator == (const FieldType& first, const FieldType& second)
{
    return first.content == second.content && first.size == second.size;
}

inline bool operator != (const FieldType& first, const FieldType& second)
{
    return ! (first == second);
}

// Umozliwia wypisywanie FieldType w czytelnym formacie (do debugowania)
inline std::ostream& operator << (std::ostream& os, const FieldType& type)
{
    os << type.content;
    if (type.size != FieldType::NON_APPLICABLE)
        os << "(" << type.size << ")";
    return os;
}


// Funkcja sprawdzajaca, czy typ jest typem zmiennej dlugosci
inline bool is_variable_size(ContentType type)
{
    return (static_cast<int>(type) & VARIABLE_MASK) != 0;
}


// Abstrakcyjna klasa bazowa dla typow danych
class Data
{  
public:
    // Zapisuje dane do bufora. Zwraca ilosc zapisanych bajtow.
    virtual size16 writeTo(OutputBinaryStream& stream) const = 0;
    
    // Wczytuje dane z podanego strumienia. Drugi argument powinien miec
    // znaczenie wylacznie dla typow zmiennej wielkosci.
    virtual void readFrom(InputBinaryStream& stream, size16 size = 0) = 0;
    
    // Zwraca rozmiar tej konkretnej instancji typu danych.
    virtual size16 size() const = 0; 
    
    // Zwraca true, jesli mapowany typ jest zmiennej wielkosci.
    bool isVariableSize() const
    {
        return is_variable_size(type().content);
    }
    
    // Zwraca mapowany typ pola
    virtual FieldType type() const = 0;
    
    // Do debugowania - wypisuje zawartosc w formacie human-readable
    virtual string toString() const = 0;
    
    virtual ~Data() { }
};


// Generyczna implementacja danej stalego rozmiaru. 
// Powinno dzialac dla wszystkich POD-ow.
template <typename U, ContentType V>
class GenericWrapper: public Data
{
private:
    typedef U Value;
    Value val;
    
public:
    explicit GenericWrapper(Value val = Value()): val(val)
    {
    }
    
    size16 writeTo(OutputBinaryStream& stream) const
    {
        stream.write(val);
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        stream.read(&val);
    }

    size16 size() const { return sizeof(val); }
    
    FieldType type() const { return { V, FieldType::NON_APPLICABLE }; }
    
    string toString() const 
    { 
        return util::format("{}", val); 
    }
};

// Dla podstawowych typow numerycznych wystarczy
typedef GenericWrapper<int32_t, ContentType::Int> Int;
typedef GenericWrapper<float, ContentType::Float> Float;
typedef GenericWrapper<page_number, ContentType::PageNumber> PageNumber;


// Stringami musimy zajac sie inaczej.
// Typ tekstowy okreslonej z gory dlugosci.
class Char: public Data
{
private:
    std::vector<char> val;
    size16 max_length;
    
public:
    explicit Char(size16 length, const string& value = ""): 
        val(length), max_length(length)
    {
        std::copy(value.begin(), value.end(), val.begin());
    }
    
    size16 writeTo(OutputBinaryStream& stream) const
    {
        stream.writeRange(val.begin(), val.end());
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        stream.readRange(val.begin(), max_length);
    }
    
    size16 size() const { return max_length; }
    FieldType type() const { return { ContentType::Char, max_length }; }
    
    string toString() const { return string(val.begin(), val.end()); }
};

// Typ tekstowy bez ograniczenia dlugosci
class VarChar: public Data
{
private:
    string val;
    
public:
    VarChar() { }
    
    VarChar(const string& val): val(val)
    {
    }
    
    size16 writeTo(OutputBinaryStream& stream) const
    {
        stream.writeRange(val.begin(), val.end());
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        val.resize(size);
        stream.readRange(val.begin(), size);
    }
    
    size16 size() const { return val.size(); }
    
    FieldType type() const 
    { 
        return { ContentType::VarChar, FieldType::NON_APPLICABLE }; 
    }
    
    string toString() const { return val; }
};


} // types
} // paganini

namespace std
{
    // Gwarantuje hashowalnosc ContentType
    template <>
    struct hash<paganini::types::ContentType>
    {
        std::size_t operator () (paganini::types::ContentType content) const
        {
            return static_cast<std::size_t>(content);
        }
    };
}

#endif // __PAGANINI_ROW_DATATYPES_H__

