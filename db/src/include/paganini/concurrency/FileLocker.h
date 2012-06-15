/*
    Klasa reprezentujaca managera lockowania pliku na poziomie jadra unixa.
*/
#ifndef __PAGANINI_CONCURRENCY_FILE_LOCKER_H__
#define __PAGANINI_CONCURRENCY_FILE_LOCKER_H__

#include <paganini/paging/configuration.h>
#include <paganini/util/format.h>
#include <paganini/concurrency/SystemError.h>
#include <fcntl.h>
#include <unordered_map>


namespace paganini
{
namespace concurrency
{


class FileLocker
{
public:
    static const int NONE = -1;
    
    // Tworzy lockera uzywajacego podanego deskryptora pliku. Domyslnie jest
    // to wartosc nieprawidlowa.
    FileLocker(int descriptor = NONE);
    
    // Zwalnia wszystkie locki
    ~FileLocker();
    
    // Ustawia deskryptor pliku, ktory chcemy blokowac
    void setFile(int descriptor);
    
    // Lockuje w trybie odczytu strone o podanym numerze 
    void pageLockRead(page_number page);
    
    // Lockuje w trybie zapisu strone o podanym numerze
    void pageLockWrite(page_number page);
    
    // Odblokowuje lock czytania
    void pageUnlockRead(page_number page);
    
    // Odblokowuje lock pisania
    void pageUnlockWrite(page_number page);
    
    // Klasa realizujaca locki - generic, statyczny polimorfizm
    template <typename Mode>
    class Lock_;
    
    struct Read_ { };
    struct Write_ { };
    
    // Klasy lockow typu read i write
    typedef Lock_<Read_> ReadLock;
    typedef Lock_<Write_> WriteLock;
    
    // Funkcje zwracajace obiekty realizujace locki w stylu RAII
    ReadLock readLock(page_number page);
    WriteLock writeLock(page_number page);
    
private:
    void lock_aux_(page_number page, short type, int cmd);
    void lock_read_(page_number page);
    void lock_write_(page_number page);
    void unlock_(page_number page);
    void unlock_all_();
    
    int descriptor_;
    struct LockInfo
    {
        size16 read_count;
        size16 write_count;
    };
    typedef std::unordered_map<page_number, LockInfo> LockMap;
    typedef LockMap::iterator LockMapIterator;
    LockMap locks_;
    
    LockInfo& findLockInfo_(page_number page);
};


// Klasa wewnetrzna, realizujaca lock w stylu RAII.
template <typename Mode>
class FileLocker::Lock_
{
private:
    FileLocker& locker_;
    page_number page_;
    
    void unlock_(Read_);
    void unlock_(Write_);
    
    void lock_(Read_);
    void lock_(Write_);
    
public:
    Lock_(FileLocker& locker, page_number page);
    Lock_(Lock_&& other);
    Lock_(const Lock_&) = delete;
    ~Lock_();
};


template <typename Mode>
FileLocker::Lock_<Mode>::Lock_(FileLocker& locker, page_number page): 
    locker_(locker), page_(page)
{
    lock_((Mode()));
}


template <typename Mode>
FileLocker::Lock_<Mode>::Lock_(Lock_&& other): locker_(other.locker_),
    page_(other.page_)
{
    other.page_ = NULL_PAGE;
}


template <typename Mode>
FileLocker::Lock_<Mode>::~Lock_()
{
    if (page_ != NULL_PAGE)
        unlock_((Mode()));
}

template <typename Mode>
void FileLocker::Lock_<Mode>::lock_(Read_)
{
    locker_.pageLockRead(page_);
}


template <typename Mode>
void FileLocker::Lock_<Mode>::lock_(Write_)
{
    locker_.pageLockWrite(page_);    
}


template <typename Mode>
void FileLocker::Lock_<Mode>::unlock_(Read_)
{
    locker_.pageUnlockRead(page_);
}


template <typename Mode>
void FileLocker::Lock_<Mode>::unlock_(Write_)
{
    locker_.pageUnlockWrite(page_);    
}


} // concurrency
} // paganini



#endif // __PAGANINI_CONCURRENCY_FILE_LOCKER_H__

