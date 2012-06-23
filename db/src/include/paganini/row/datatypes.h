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
#include <vector>
#include <sstream>
#include <functional>
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
    FieldType,
    // Odtad typy zmiennej wielkosci
    VarChar = VARIABLE_MASK
};


inline std::ostream& operator << (std::ostream& os, types::ContentType content)
{
    using types::ContentType;
    const char* str;
    switch (content)
    {
    case ContentType::None: str = "none"; break;
    case ContentType::Int: str = "int"; break;
    case ContentType::Float: str = "float"; break;
    case ContentType::PageNumber: str = "page_number"; break;
    case ContentType::Char: str = "char"; break;
    case ContentType::FieldType: str = "field_type"; break;
    case ContentType::VarChar: str = "varchar"; break;
    default: str = "INVALID";
    }
    return os << str;
}


// Pelny opis typu danych - rodzaj + wielkosc (dla char).
struct FieldType
{
    static const size16 NON_APPLICABLE = 0;
    ContentType content;
    size16 size;
    
    FieldType(ContentType content = ContentType::None, 
        size16 size = NON_APPLICABLE): content(content), size(size)
    {
    }
};


// Umozliwia wypisywanie FieldType w czytelnym formacie (do debugowania)
inline std::ostream& operator << (std::ostream& os, 
    const types::FieldType& type)
{
    using types::FieldType;
    os << type.content;
    if (type.size != FieldType::NON_APPLICABLE)
        os << "(" << type.size << ")";
    return os;
}


// Operatory do FieldType
inline bool operator == (const FieldType& first, const FieldType& second)
{
    return first.content == second.content && first.size == second.size;
}

inline bool operator != (const FieldType& first, const FieldType& second)
{
    return ! (first == second);
}


// Funkcja sprawdzajaca, czy typ jest typem zmiennej dlugosci
inline bool is_variable_size(ContentType type)
{
    return (static_cast<int>(type) & VARIABLE_MASK) != 0;
}


// Abstrakcyjna klasa bazowa dla typow danych. Wszystkie typy dziedziczace
// z niej powinny udostepniac rowniez metode value zwracajaca wartosc
// przechowywana.
class Data
{  
public:
    // Zapisuje dane do bufora.
    virtual void writeTo(OutputBinaryStream& stream) const = 0;
    
    // Wczytuje dane z podanego strumienia. Drugi argument powinien miec
    // znaczenie wylacznie dla typow zmiennej wielkosci.
    virtual void readFrom(InputBinaryStream& stream, size16 size = 0) = 0;
    
    // Wczytuje dane w formie tekstowej z podanego stringa. Zwraca true,
    // jesli sie udalo i false, jesli format stringa byl niewlasciwy.
    virtual bool readFrom(const string& str) = 0;
    
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
    
    void writeTo(OutputBinaryStream& stream) const
    {
        stream.write(val);
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        stream.read(&val);
    }
    
    bool readFrom(const string& str)
    {
        std::stringstream ss(str);
        Value copy;;
        ss >> copy;
        if (! ss)
            return false;
        else
        {
            val = copy;
            return true;
        }
    }

    size16 size() const { return sizeof(val); }
    
    FieldType type() const { return { V, FieldType::NON_APPLICABLE }; }
    
    Value value() const { return val; }
    
    string toString() const
    { 
        return util::format("{}", val); 
    }
};

// Dla podstawowych typow numerycznych wystarczy
typedef GenericWrapper<int32_t, ContentType::Int> Int;
typedef GenericWrapper<float, ContentType::Float> Float;
typedef GenericWrapper<page_number, ContentType::PageNumber> PageNumber;

// Reprezentacja typu pola
class FieldTypeData: public Data
{
private:
    FieldType type_;
    
public:
    explicit FieldTypeData(FieldType type = FieldType()): type_(type)
    {
    }
    
    void writeTo(OutputBinaryStream& stream) const
    {
        stream.write(static_cast<size16>(type_.content));
        stream.write(type_.size);
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        size16 content;
        stream.read(&content);
        type_.content = static_cast<ContentType>(content);
        stream.read(&type_.size);
    }
    
    // TODO: Dorobic, ale nie jest potrzebne
    bool readFrom(const string& str)
    {
        return false;
    }
    
    size16 size() const { return sizeof(type_); }
    
    FieldType type() const 
    { 
        return  { ContentType::FieldType, FieldType::NON_APPLICABLE }; 
    }
    
    FieldType value() const { return type_; }
    
    string toString() const { return util::format("{}", value());  }
};


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
    
    explicit Char(const std::pair<string, size16>& pair): val(pair.second),
        max_length(pair.second)
    {
        const string& value = pair.first;
        std::copy(value.begin(), value.end(), val.begin());
    }
    
    void writeTo(OutputBinaryStream& stream) const
    {
        stream.writeRange(val.begin(), val.end());
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        val.assign(max_length, '\0');
        stream.readRange(val.begin(), max_length);
    }
    
    bool readFrom(const string& str)
    {
        if (str.length() <= max_length)
        {
            val.assign(max_length, '\0');
            std::copy(str.begin(), str.end(), val.begin());
            return true;
        }
        else
            return false;
    }
    
    size16 size() const { return max_length; }
    
    FieldType type() const { return { ContentType::Char, max_length }; }
    
    string value() const { return string(val.begin(), val.end()); }
    
    string toString() const { return value(); }
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
    
    void writeTo(OutputBinaryStream& stream) const
    {
        stream.writeRange(val.begin(), val.end());
    }
    
    void readFrom(InputBinaryStream& stream, size16 size = 0)
    {
        val.resize(size);
        stream.readRange(val.begin(), size);
    }
    
    bool readFrom(const string& str)
    {
        val = str;
        return true;
    }
    
    size16 size() const { return val.size(); }
    
    FieldType type() const 
    { 
        return { ContentType::VarChar, FieldType::NON_APPLICABLE }; 
    }
    
    const string& value() const { return val; }
    
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

