/*
    Klasa reprezentujaca managera lockowania stron na poziomie jadra unixa.
    Przeznaczona do uzytku w przypadku zdecentralizowanym, umozliwia
    synchronizacje miedzy procesami.
*/
#ifndef __PAGANINI_CONCURRENCY_FILE_PAGE_LOCKER_H__
#define __PAGANINI_CONCURRENCY_FILE_PAGE_LOCKER_H__

#include <paganini/paging/configuration.h>
#include <paganini/concurrency/GenericPageLocker.h>
#include <unordered_map>


namespace paganini
{
namespace concurrency
{


class FilePageLocker: public GenericPageLocker<FilePageLocker>
{
public:
    static const int NONE = -1;
    
    // Tworzy lockera uzywajacego podanego deskryptora pliku. Domyslnie jest
    // to wartosc nieprawidlowa.
    FilePageLocker(int descriptor = NONE);
    
    // Move-constructible
    FilePageLocker(FilePageLocker&& other);
    
    // Zwalnia wszystkie locki
    ~FilePageLocker();
    
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
    
    LockInfo& find_lock_info_(page_number page);
};


} // concurrency
} // paganini

#endif // __PAGANINI_CONCURRENCY_FILE_PAGE_LOCKER_H__

