/*
    Szablon zawierajacy boiler-plate code do managerow blokowania stron.
    Minimalizuje kod konieczny do napisania w celu zaimplementowania nowego
    lockera.
*/
#ifndef __PAGANINI_CONCURRENCY_GENERIC_PAGE_LOCKER_H__
#define __PAGANINI_CONCURRENCY_GENERIC_PAGE_LOCKER_H__

#include <paganini/concurrency/PageLock.h>
#include <type_traits>


namespace paganini
{
namespace concurrency
{


template <class _RealLocker>
class GenericPageLocker
{
public:
    // Klasy lockow typu read i write
    typedef PageLock<_RealLocker, details::Read> ReadLock;
    typedef PageLock<_RealLocker, details::Write> WriteLock;
    
    // Funkcje zwracajace obiekty realizujace locki w stylu RAII
    ReadLock readLock(page_number page);    
    WriteLock writeLock(page_number page);
    
private:
    typedef GenericPageLocker<_RealLocker> self;
};


template <class _RealLocker>
typename GenericPageLocker<_RealLocker>::ReadLock 
GenericPageLocker<_RealLocker>::readLock(page_number page)
{
    static_assert(std::is_base_of<self, _RealLocker>::value,
        "Class A should inherit GenericLocker<A>");
    return { static_cast<_RealLocker&>(*this), page };
}


template <class _RealLocker>
typename GenericPageLocker<_RealLocker>::WriteLock 
GenericPageLocker<_RealLocker>::writeLock(page_number page)
{
    static_assert(std::is_base_of<self, _RealLocker>::value,
        "Class A should inherit GenericLocker<A>");
    return { static_cast<_RealLocker&>(*this), page };
}


} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_GENERIC_PAGE_LOCKER_H__

