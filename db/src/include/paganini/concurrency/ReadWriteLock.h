/*
    Prosta realizacja locka rozwiazujacego problem czytelnikow i pisarzy.
    Implementacja korzysta z prymitywow: mutexa i semafora. Jest to wersja
    niefaworyzujaca zadnej ze stron. Nie wspiera wielokrotnego lockowania
    ani upgradeowania locka.
*/
#ifndef __PAGANINI_CONCURRENCY_READ_WRITE_LOCK_H__
#define __PAGANINI_CONCURRENCY_READ_WRITE_LOCK_H__

#include <paganini/concurrency/ScopeLock.h>

#include <iostream>


namespace paganini
{
namespace concurrency
{

// _Semaphore - klasa reprezentujaca semafor, powinna posiadac konstruktor
//    jednoargumentowy przyjmujacy wartosc poczatkowa
template <class _Mutex, class _Semaphore>
class ReadWriteLock
{
public:
    ReadWriteLock();
    
    ReadWriteLock(const ReadWriteLock&) = delete;
    
    // Blokuje w trybie czytania
    void lockRead();
    
    // Odblokowuje w trybie czytania
    void unlockRead();
    
    // Blokuje w trybie pisania
    void lockWrite();
    
    // Odblokowuje w trybie pisania
    void unlockWrite();
    
    // Zwraca true jesli biblioteka jest pusta
    bool empty();

private:
    _Semaphore room_empty_;
    _Mutex no_writers_;
    _Mutex mutex_;
    int reader_count_;
    bool writer_;
};


template <class _Mutex, class _Semaphore>
ReadWriteLock<_Mutex, _Semaphore>::ReadWriteLock(): room_empty_(1),
    reader_count_(0)
{
}


template <class _Mutex, class _Semaphore>
void ReadWriteLock<_Mutex, _Semaphore>::lockRead()
{
    int prev_count = 0;
    auto no_writers_lock = make_lock(no_writers_);
    {
        auto mutex_lock = make_lock(mutex_);
        prev_count = reader_count_ ++;
    }
    if (prev_count == 0)
        room_empty_.lock();
}


template <class _Mutex, class _Semaphore>
void ReadWriteLock<_Mutex, _Semaphore>::unlockRead()
{   
    int count = 0;
    {
        auto mutex_lock = make_lock(mutex_);
        count = -- reader_count_;
    }
    if (count == 0)
        room_empty_.unlock();
}


template <class _Mutex, class _Semaphore>
void ReadWriteLock<_Mutex, _Semaphore>::lockWrite()
{
    auto no_writers_lock = make_lock(no_writers_);
    room_empty_.lock();
    writer_ = true;
}


template <class _Mutex, class _Semaphore>
void ReadWriteLock<_Mutex, _Semaphore>::unlockWrite()
{
    writer_ = false;
    room_empty_.unlock();
}


template <class _Mutex, class _Semaphore>
bool ReadWriteLock<_Mutex, _Semaphore>::empty()
{
    // TODO: Jak sie spieprzy, to moze dlatego
    // auto no_writers_lock = make_lock(no_writers_);
    return (! writer_ && reader_count_ == 0);
}


} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_READ_WRITE_LOCK_H__

