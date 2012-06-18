/*
    Klasa reprezentujaca manager lockowania stron w przypadku scentralizowanym.
    Umozliwia synchronizacje watkow dzialajacych w obrebie jednego procesu.
*/
#ifndef __PAGANINI_CONCURRENCY_THREAD_PAGE_LOCKER_H__
#define __PAGANINI_CONCURRENCY_THREAD_PAGE_LOCKER_H__

#include <paganini/paging/configuration.h>
#include <paganini/concurrency/GenericPageLocker.h>


namespace paganini
{
namespace concurrency
{


template <class _Mutex, class _ReadWriteLock>
class ThreadPageLocker: 
    public GenericPageLocker<
        ThreadPageLocker<_Mutex, _ReadWriteLock>
    >
{
private:
    _Mutex mutex_;
};


} // concurrency
} // paganini

#endif // __PAGANINI_CONCURRENCY_THREAD_PAGE_LOCKER_H__

