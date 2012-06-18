/*
    Ogolna klasa reprezentujaca lock na stronie pliku. Jest parametryzowana 
    typem (read/write) i typem managera lockow. 
*/
#ifndef __PAGANINI_CONCURRENCY_PAGE_LOCK_H__
#define __PAGANINI_CONCURRENCY_PAGE_LOCK_H__

#include <paganini/paging/types.h>


namespace paganini
{
namespace concurrency
{

namespace details
{
    struct Read { };
    struct Write { };
}


template <class _Locker, typename _Mode>
class PageLock
{
private:
    _Locker& locker_;
    page_number page_;
    
    void unlock_(details::Read);
    void unlock_(details::Write);
    
    void lock_(details::Read);
    void lock_(details::Write);
    
public:
    PageLock(_Locker& locker, page_number page);
    PageLock(PageLock&& other);
    PageLock(const PageLock&) = delete;
    ~PageLock();
};


template <class _Locker, typename _Mode>
PageLock<_Locker, _Mode>::PageLock(_Locker& locker, page_number page): 
    locker_(locker), page_(page)
{
    lock_((_Mode()));
}


template <class _Locker, typename _Mode>
PageLock<_Locker, _Mode>::PageLock(PageLock&& other): 
    locker_(other.locker_), page_(other.page_)
{
    other.page_ = NULL_PAGE;
}


template <class _Locker, typename _Mode>
PageLock<_Locker, _Mode>::~PageLock()
{
    if (page_ != NULL_PAGE)
        unlock_((_Mode()));
}

template <class _Locker, typename _Mode>
void PageLock<_Locker, _Mode>::lock_(details::Read)
{
    locker_.pageLockRead(page_);
}


template <class _Locker, typename _Mode>
void PageLock<_Locker, _Mode>::lock_(details::Write)
{
    locker_.pageLockWrite(page_);    
}


template <class _Locker, typename _Mode>
void PageLock<_Locker, _Mode>::unlock_(details::Read)
{
    locker_.pageUnlockRead(page_);
}


template <class _Locker, typename _Mode>
void PageLock<_Locker, _Mode>::unlock_(details::Write)
{
    locker_.pageUnlockWrite(page_);    
}


} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_PAGE_LOCK_H__
