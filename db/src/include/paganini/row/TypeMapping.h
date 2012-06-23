/*
    Plik zawiera konfiguracje mapowania typow bazodanowych na podklasy Data,
    ktore im odpowiadaja.
*/
#ifndef __PAGANINI_ROW_TYPE_MAPPING_H__
#define __PAGANINI_ROW_TYPE_MAPPING_H__

#include <paganini/row/datatypes.h>
#include <utility>


namespace paganini
{

namespace types
{

// Struktura statycznie przechowujaca informacje o typach uzywanych do
// mapowania typow bazodanowych. Specjalizacje okreslaja ja poprzez swojego
// typedefa o nazwie type.
template <ContentType Type>
struct TypeMapping;


// Konfiguracja kolejnych typow
template <>
struct TypeMapping<ContentType::Int>
{
    typedef Int type;
    typedef int content_type;
};


template <>
struct TypeMapping<ContentType::Float>
{
    typedef Float type;
    typedef float content_type;
};


template <>
struct TypeMapping<ContentType::PageNumber>
{
    typedef PageNumber type;
    typedef page_number content_type;
};


template <>
struct TypeMapping<ContentType::Char>
{
    typedef Char type;
    typedef string content_type;
};


template <>
struct TypeMapping<ContentType::VarChar>
{
    typedef VarChar type;
    typedef string content_type;
};


template <>
struct TypeMapping<ContentType::FieldType>
{
    typedef FieldTypeData type;
    typedef FieldType content_type;
};


// Odwzorowanie pomiedzy typami C++ a typami bazodanowymi
template <typename T>
struct DatabaseType;


template <>
struct DatabaseType<int>
{
    typedef Int type;
};



template <>
struct DatabaseType<float>
{
    typedef Float type;
};


/*
template <>
struct DatabaseType<page_number>
{
    typedef PageNumber type;
};
*/


template <>
struct DatabaseType<string>
{
    typedef VarChar type;
};


template <>
struct DatabaseType<FieldType>
{
    typedef FieldTypeData type;
};


template <>
struct DatabaseType<std::pair<string, size16>>
{
    typedef Char type;
};



template <typename T>
//std::unique_ptr<typename DatabaseType<T>::type>
typename DatabaseType<T>::type*
make_field(const T& val)
{
    return new typename DatabaseType<T>::type(val);
}


// Oddzielne overloady dla problematycznych przypadkow
inline DatabaseType<string>::type*
make_field(const string& val)
{
    return new DatabaseType<string>::type(val);
}


//inline DatabaseType<std::pair<string, size16>>::type*
inline Char* make_field(size16 length, const string& value)
{
    return new Char(length, value);
}


template <ContentType Content>
typename TypeMapping<Content>::content_type data_cast(const Data& data)
{
    return dynamic_cast<const typename TypeMapping<Content>::type&>(data).value();
}


} // types
} // paganini

#endif // __PAGANINI_ROW_TYPE_MAPPING_H__
