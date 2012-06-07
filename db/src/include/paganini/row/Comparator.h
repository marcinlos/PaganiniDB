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
    // Zwraca true jesli a < b
    virtual bool operator () (const DataType& a, const DataType& b) const = 0;
    
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
    
    bool operator () (const BaseType& a, const BaseType& b) const;
};


template <types::ContentType Type, class CastingPolicy>
bool DataComparator<Type, CastingPolicy>::operator ()(const BaseType& a, 
    const BaseType& b) const
{
    CastingPolicy cast;
    const ComparedType& aa = cast.template operator()<ComparedType>(a);
    const ComparedType& bb = cast.template operator()<ComparedType>(b);
    
    return aa.value() < bb.value();
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
