/*
    Tablica z odwroconym indeksowaniem - przekazywany w konstruktorze iterator
    wskazywac powinien za koniec interesujacego nas ciagu.
*/
#ifndef __PAGANINI_UTIL_REVERSE_ARRAY_H__
#define __PAGANINI_UTIL_REVERSE_ARRAY_H__
#include <iterator>

namespace paganini
{

namespace util
{


template <typename Iterator>
class ReverseArray
{
public:
    typedef typename std::iterator_traits<Iterator>::reference reference;
    typedef typename std::iterator_traits<Iterator>::difference_type 
        difference_type;
    ReverseArray(Iterator end);
    
    // Operatory indeksowania
    inline reference operator [] (difference_type n) const;
    
private:
    Iterator end_;
};


template <typename Iterator>
ReverseArray<Iterator>::ReverseArray(Iterator end): end_(end)
{
}


template <typename Iterator>
typename ReverseArray<Iterator>::reference
ReverseArray<Iterator>::operator [] (difference_type n) const
{
    return *(end_ - 1 - n);
}


} // util
} // paganini

#endif // __PAGANINI_UTIL_REVERSE_ARRAY_H__
