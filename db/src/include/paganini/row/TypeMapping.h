/*
    Plik zawiera konfiguracje mapowania typow bazodanowych na podklasy Data,
    ktore im odpowiadaja.
*/
#ifndef __PAGANINI_ROW_TYPE_MAPPING_H__
#define __PAGANINI_ROW_TYPE_MAPPING_H__

#include <paganini/row/datatypes.h>


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
};


template <>
struct TypeMapping<ContentType::Float>
{
    typedef Float type;
};


template <>
struct TypeMapping<ContentType::PageNumber>
{
    typedef PageNumber type;
};


template <>
struct TypeMapping<ContentType::Char>
{
    typedef Char type;
};


template <>
struct TypeMapping<ContentType::VarChar>
{
    typedef VarChar type;
};


} // types
} // paganini

#endif // __PAGANINI_ROW_TYPE_MAPPING_H__
