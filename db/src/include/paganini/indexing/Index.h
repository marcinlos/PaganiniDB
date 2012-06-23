/*
    Klasa reprezentujaca dane pojedynczego indeksu.
*/
#ifndef __PAGANINI_INDEXING_INDEX_H__
#define __PAGANINI_INDEXING_INDEX_H__

#include <paganini/row/datatypes.h>
#include <paganini/util/format.h>
#include <iostream>
#include <memory>


namespace paganini
{


class Index
{
public:
    typedef std::shared_ptr<types::Data> DataPtr;
    typedef std::shared_ptr<const types::Data> ConstDataPtr;
     
    Index(const types::FieldType& type = types::FieldType(), 
        ConstDataPtr value = nullptr, page_number child = 0);
        
    // Copy-constructible
    Index(const Index& other);
    
    // Copy-assignable
    Index& operator = (const Index& other);

    // Zwraca przechowywana wartosc indeksu
    inline ConstDataPtr value() const;
    
    // Zwraca numer strony, na ktora pokazuje indeks
    inline page_number child() const;
    
    // Zwraca typ pola indeksowanego
    inline const types::FieldType& type() const;
    
private:
    types::FieldType type_;
    ConstDataPtr value_;
    page_number child_;
};


Index::ConstDataPtr Index::value() const
{
    return value_;
}


page_number Index::child() const
{
    return child_;
}


const types::FieldType& Index::type() const
{
    return type_;
}


inline std::ostream& operator << (std::ostream& os, const Index& index)
{
    return os << util::format("[{}]: {} (-> {})", index.type(), 
        index.value()->toString() ,index.child());
}


}


#endif // __PAGANINI_INDEXING_INDEX_H__
