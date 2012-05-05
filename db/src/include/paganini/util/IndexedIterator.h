/*
    'Zlozenie' dwoch iteratorow: jeden opisuje sama kolekcje, po ktorej
    iterujemy, drugi kolekcje indeksow do niej.
    Iterator jest random access, jesli tylko iterator
    indeksow rowniez jest tego rodzaju.
*/
#ifndef __PAGANINI_UTIL_INDEXED_ITERATOR_H__
#define __PAGANINI_UTIL_INDEXED_ITERATOR_H__

#include <iterator>


namespace paganini
{

namespace util
{

template <typename IndexIterator, typename ValueIterator>
struct IndexedIterator
{
private:
    IndexIterator i;
    ValueIterator begin;
    
public:

    typedef std::iterator_traits<ValueIterator> ValueTraits;
    typedef std::iterator_traits<IndexIterator> IndexTraits;
    typedef typename IndexTraits::iterator_category iterator_category;
    typedef typename ValueTraits::value_type value_type;
    typedef typename ValueTraits::difference_type difference_type;
    typedef typename ValueTraits::pointer pointer;
    typedef typename ValueTraits::reference reference;
          
    IndexedIterator(IndexIterator indexBegin, ValueIterator valueBegin): 
        i(indexBegin), begin(valueBegin)
    {
    }
    
    // Inkrementacja/dekrementacja - wystarczy uczynic to na iteratorze
    // indeksow.
    IndexedIterator& operator ++ ()
    {
        ++ i;
        return *this;
    }
    
    IndexedIterator& operator -- ()
    {
        -- i;
        return *this;
    }
    
    // Dereferencje i wyluskanie tez mozna latwo uzyskac przy uzyciu dwoch
    // iteratorow, ktore mamy.
    reference operator * () const
    {
        return *(begin + (*i));
    }
    
    ValueIterator operator -> () const
    {
        return begin + (*i);
    }
    
    // Arytmetyka
    IndexedIterator operator + (difference_type n) const
    {
        return {i + n, begin};
    }
    
    // Porownania
    friend bool operator == (const IndexedIterator& lhs,
        const IndexedIterator& rhs)
    {
        return (lhs.begin == rhs.begin && lhs.i == rhs.i);
    }
    
    friend bool operator != (const IndexedIterator& lhs,
        const IndexedIterator& rhs)
    {
        return ! (lhs == rhs);
    }
};

} // util
} // paganini


#endif // __PAGANINI_UTIL_INDEXED_ITERATOR_H__
