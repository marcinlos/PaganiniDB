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
     
    Index(types::FieldType type, types::Data* value, page_number child);

    // Zwraca przechowywana wartosc indeksu
    inline DataPtr value();
    inline ConstDataPtr value() const;
    
    // Zwraca numer strony, na ktora pokazuje indeks
    inline page_number child() const;
    
    // Zwraca typ pola indeksowanego
    inline const types::FieldType& type() const;
    
private:
    types::FieldType type_;
    DataPtr value_;
    page_number child_;
};


Index::DataPtr Index::value()
{
    return value_;
}


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