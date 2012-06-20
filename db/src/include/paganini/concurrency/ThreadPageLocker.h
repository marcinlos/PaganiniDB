/*
    Klasa reprezentujaca manager lockowania stron w przypadku scentralizowanym.
    Umozliwia synchronizacje watkow dzialajacych w obrebie jednego procesu.
*/
#ifndef __PAGANINI_CONCURRENCY_THREAD_PAGE_LOCKER_H__
#define __PAGANINI_CONCURRENCY_THREAD_PAGE_LOCKER_H__

#include <paganini/paging/types.h>
#include <paganini/paging/configuration.h>
#include <paganini/util/format.h>
#include <paganini/concurrency/GenericPageLocker.h>
#include <paganini/concurrency/ScopeLock.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <stdexcept>


namespace paganini
{
namespace concurrency
{


template <
    class _Thread, 
    class _Mutex = typename _Thread::Mutex,
    class _ReadWriteLock = typename _Thread::ReadWriteLock
>
class ThreadPageLocker: 
    public GenericPageLocker<
        ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>
    >
{
public:
    ThreadPageLocker() {}
    ThreadPageLocker(const ThreadPageLocker&) = delete;

    // Lockuje w trybie odczytu strone o podanym numerze 
    void pageLockRead(page_number page);
    
    // Lockuje w trybie zapisu strone o podanym numerze
    void pageLockWrite(page_number page);
    
    // Odblokowuje lock czytania
    void pageUnlockRead(page_number page);
    
    // Odblokowuje lock pisania
    void pageUnlockWrite(page_number page);    
    
private:
    _Mutex mutex_;
    
    // Pula lockow, z ktorej przydzielamy do konkretnych stron
    std::vector<std::unique_ptr<_ReadWriteLock>> lock_pool_;
    
    // Informacje o lockach danego watku
    struct LockInfo
    {
        typename _Thread::Id tid;
        size16 read_count;
        size16 write_count;
    };
    
    struct PageState
    {
        // wartosc > 0: ilosc czytelnikow
        // wartos 0: pusta
        // wartosc -1: pisarz
        short accessors;    
            
        // Indeks do puli lockow. -1 gdy nie jest przydzielony
        int lock_index;
        
        // Lista trzymanych przez watki blokowan
        std::list<LockInfo> lock_list;
    };
    
    std::unordered_map<page_number, PageState> pages_;
    
    PageState& find_page_state_(page_number page);
    
    int find_free_lock_();
    
    LockInfo& find_lock_info_(PageState& state);
};


template <class _Thread, class _Mutex, class _ReadWriteLock>
typename ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::PageState& 
ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::find_page_state_(
    page_number page)
{
    auto i = pages_.find(page);
    if (i == pages_.end())
    {
        int free_lock = find_free_lock_();
        i = pages_.insert({page, {0, free_lock, {}}}).first;
    }
    else if (i->second.lock_index < 0)
    {
        // W razie potrzeby przywracamy przydzial locka
        i->second.lock_index = find_free_lock_();
    }
    return i->second;
}


template <class _Thread, class _Mutex, class _ReadWriteLock>
typename ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::LockInfo& 
ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::find_lock_info_(
    PageState& state)
{
    typename _Thread::Id id = _Thread::self();
    auto i = std::find_if(
        state.lock_list.begin(), 
        state.lock_list.end(),
        [id](const LockInfo& info) { return info.tid == id; }
    );
    if (i == state.lock_list.end())
    {
        state.lock_list.push_front({id, 0, 0});
        i = state.lock_list.begin();
    }
    return *i;
}


template <class _Thread, class _Mutex, class _ReadWriteLock>
int ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::find_free_lock_()
{
    for (unsigned i = 0; i < lock_pool_.size(); ++ i)
    {
        if (lock_pool_[i]->empty())
            return i;
    }
    lock_pool_.emplace_back(new _ReadWriteLock);
    return lock_pool_.size() - 1;
}


template <class _Thread, class _Mutex, class _ReadWriteLock>
void ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::pageLockRead(
    page_number page)
{
    bool do_lock = false;
    int lock_index = -1;
    {
        auto mutex_lock = make_lock(mutex_);
        PageState& state = find_page_state_(page);
        lock_index = state.lock_index;
        LockInfo& info = find_lock_info_(state);
        ++ info.read_count;
        do_lock = info.read_count == 1 && info.write_count == 0;
        if (do_lock)
            ++ state.accessors;
    }
    
    // Jesli to pierwszy lock, to istotnie lockujemy
    if (do_lock)
        lock_pool_[lock_index]->lockRead();
}


template <class _Thread, class _Mutex, class _ReadWriteLock>
void ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::pageUnlockRead(
    page_number page)
{
    
    auto mutex_lock = make_lock(mutex_);
    PageState& state = find_page_state_(page);
    LockInfo& info = find_lock_info_(state);
    
    if (info.read_count > 0)
    {
        -- info.read_count;
        if (info.read_count == 0 && info.write_count == 0)
        {
            lock_pool_[state.lock_index]->unlockRead();
            -- state.accessors;
            // Odbieramy locka, to pewnie bedzie nieefektywne
            // state.lock_index = -1;
        }
    }
    else
    {
        throw std::logic_error(util::format("Trying to unlock page {}, "
            "which is not locked for reading", page));
    }
}



template <class _Thread, class _Mutex, class _ReadWriteLock>
void ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::pageLockWrite(
    page_number page)
{
    bool do_lock = false;
    int lock_index = -1;
    {
        auto mutex_lock = make_lock(mutex_);
        PageState& state = find_page_state_(page);
        lock_index = state.lock_index;
        LockInfo& info = find_lock_info_(state);
        ++ info.write_count;
        
        if (info.write_count == 1)
        {
            if (info.read_count == 0)
                do_lock = true;
            else // proba upgrade'owania
            {
                throw std::logic_error(util::format("Trying to upgrade lock on "
                    "page {}; this lock is not upgradable", page));
            }
        }
    }
    
    // Jesli to pierwszy lock typu write, to istotnie lockujemy
    if (do_lock)
    {
        lock_pool_[lock_index]->lockWrite();
    }
}


template <class _Thread, class _Mutex, class _ReadWriteLock>
void ThreadPageLocker<_Thread, _Mutex, _ReadWriteLock>::pageUnlockWrite(
    page_number page)
{
    auto mutex_lock = make_lock(mutex_);
    PageState& state = find_page_state_(page);
    LockInfo& info = find_lock_info_(state);
    if (info.write_count > 0)
    {
        -- info.write_count;
        if (info.write_count == 0)
        {
            if (info.read_count == 0)
            {
                lock_pool_[state.lock_index]->unlockWrite();
                // Odbieramy locka, to pewnie bedzie nieefektywne
                // state.lock_index = -1;
            }
            else
            {
                throw std::logic_error(util::format("Trying to downgrade "
                    "lock on page {}; this lock is not downgradable", page));
            }
        }
    }
    else
    {
        throw std::logic_error(util::format("Trying to unlock page {}, "
            "which is not locked for writing", page));
    }
}



} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_THREAD_PAGE_LOCKER_H__

