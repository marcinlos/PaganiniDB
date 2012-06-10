/*
    Klasy sluzace do porownywania reprezentacji typow bazodanowych.
*/
#ifndef __PAGANINI_ROW_COMPARATOR_H__
#define __PAGANINI_ROW_COMPARATOR_H__

#include <paganini/row/datatypes.h>
#include <paganini/row/TypeMapping.h>
#include <paganini/util/format.h>
#include <stdexcept>


namespace paganini
{


// Abstrakcyjna klasa bazowa dla komparatorow danych :(
template <typename DataType>
class Comparator
{
public:
    // Zwraca liczbe < 0 jesli a < b, 0 jesli a = b, >0 jesli a > b
    virtual int operator () (const DataType& a, const DataType& b) const = 0;
    
    virtual ~Comparator() = 0;
};


template <typename DataType>
Comparator<DataType>::~Comparator()
{
}


// Generyczna implementacja uzywajaca typow skonfigurowanych w TypeMapping.h.
// Wymaga rzutowania. Jego sposob okresla parametr CastingPolicy. Powinien on
// udostepniac szablon operatora (), pobierajacy dwa parametry: typ zrodlowy
// oraz docelowy, a takze przyjmowac obiekt typu zrodlowego i zwracac obiekt
// badz referencje typu docelowego.
template <types::ContentType Type, class CastingPolicy>
class DataComparator: public Comparator<types::Data>
{
public:
    typedef types::Data BaseType;
    typedef typename types::TypeMapping<Type>::type ComparedType;
    
    template <typename U, typename V>
    inline static int compare(const U& u, const V& v);
    
    int operator () (const BaseType& a, const BaseType& b) const;
};


template <types::ContentType Type, class CastingPolicy>
template <typename U, typename V>
int DataComparator<Type, CastingPolicy>::compare(const U& u, const V& v)
{
    return u < v ? -1 : u == v ? 0 : 1;
}


template <types::ContentType Type, class CastingPolicy>
int DataComparator<Type, CastingPolicy>::operator ()(const BaseType& a, 
    const BaseType& b) const
{
    CastingPolicy cast;
    const ComparedType& aa = cast.template operator()<ComparedType>(a);
    const ComparedType& bb = cast.template operator()<ComparedType>(b);
    
    return compare(aa.value(), bb.value());
}


// Przykladowe polityki rzutowania - bezpieczna (DynamicCastPolicy),
// oraz efektywniejsza - StaticCastPolicy

struct DynamicCastPolicy
{
    template <typename Dest, typename Src>
    const Dest& operator () (const Src& a) const
    {
        return dynamic_cast<const Dest&>(a);
    }
};


struct StaticCastPolicy
{
    template <typename Dest, typename Src>
    const Dest& operator () (const Src& a) const
    {
        return static_cast<const Dest&>(a);
    }
};



}

#endif // __PAGANINI_ROW_COMPARATOR_H__
