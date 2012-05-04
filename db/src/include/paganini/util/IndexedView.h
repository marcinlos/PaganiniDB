/*
    Szablon do kontenerow indeksowanych - jeden kontener zawiera wartosci,
    a drugi indeksy do pierwszego. Obecnie dostepny jest jedynie widok
    niemodyfikowalny.
*/
#ifndef __PAGANINI_UTIL_INDEXED_VIEW_H__
#define __PAGANINI_UTIL_INDEXED_VIEW_H__

#include <paganini/util/IndexedIterator.h>
#include <iterator>


namespace paganini
{

namespace util
{


template <typename IndexIterator, typename ValueIterator>
class IndexedView
{
private:
    IndexIterator _begin, _end;
    ValueIterator _values;
    
public:
    typedef IndexedIterator<IndexIterator, ValueIterator> const_iterator;

    IndexedView(IndexIterator begin, IndexIterator end, ValueIterator values):
        _begin(begin), _end(end), _values(values)
    {
    }
    
    // Jedynie wersje stale - nie ma mozliwosci zmiany danych.
    const_iterator begin() const
    {
        return const_iterator(_begin, _values);
    }
    
    const_iterator end() const
    {
        return const_iterator(_end, _values);
    }
    
    typename const_iterator::reference 
    operator [] (typename const_iterator::difference_type n) const
    {
        return *(_values + (*(_begin + n)));
    }
};


} // util
} // paganini


#endif // __PAGANINI_UTIL_INDEXED_VIEW_H__
