/*
    Klasa realizujaca zadania wymagane od PersistenceManagera w PageManagerze
    poprzez zapisywanie danych na plik dyskowy. 
    Locker parametryzuje informacje zwiazane ze sposobem lockowania stron.
    Powinien udostepniac nastepujace typy:
    - Locker::ReadLock
    - Locker::WriteLock
    Powinny one byc move-constructible. Ponadto powinien udostepniac metody:
    - readLock(page_number page)
    - writeLock(page_number page)
*/
#ifndef __PAGANINI_PAGING_FILE_PERSISTENCE_MANAGER_H__
#define __PAGANINI_PAGING_FILE_PERSISTENCE_MANAGER_H__

#include <paganini/paging/types.h>
#include <paganini/Error.h>
#include <paganini/paging/PageBuffer.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/util/format.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string>
using std::string;


namespace paganini
{


template <class Locker>
class FilePersistenceManager
{
public:
    FilePersistenceManager(const Locker& locker = Locker());
    
    ~FilePersistenceManager();
    
    // Tworzy nowy plik w podanej lokacji
    void create(const string& path);

    // Otwiera do czytania i pisania plik z podanej lokacji
    void open(const string& path);

    // Konczy dzialanie managera stronnicowania
    void close();
    
    // Zapewnia istnienie stron o numerach nie wiekszych niz podana
    void ensurePages(page_number number);
    
    // Wczytuje do podanego bufora dane ze strony o podanym numerze
    void read(page_number number, PageBuffer* page);
    
    // Zapisuje dane strony
    void write(page_number number, const PageBuffer* page);
    
    // Typy lockow
    typedef typename Locker::ReadLock ReadLock;
    typedef typename Locker::WriteLock WriteLock;
    
    // Tworzy lock czytelnika na podana strone
    ReadLock readLock(page_number page);
    
    // Tworzy lock pisarza na podana strone
    WriteLock writeLock(page_number page);
    
private:
    void moveToPage_(page_number page);
    void passFdToLocker_();
    
    Locker locker_;
    const static int EMPTY_FD = -1;
    int fd_;
};


template <class Locker>
FilePersistenceManager<Locker>::FilePersistenceManager(const Locker& locker):
    locker_(locker)
{
    fd_ = EMPTY_FD;
}


template <class Locker>
FilePersistenceManager<Locker>::~FilePersistenceManager()
{
    if (fd_ != EMPTY_FD)
    {
        close();
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::create(const string& path)
{
    // Tworzymy nowy plik dostepny dla uzytkownika
    if ((fd_ = ::open(path.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR)) < 0)
    {
        throw Exception(util::format("Trying to create file '{}'", path), 
            Error::FILECREATE);
    }
    passFdToLocker_();
}


template <class Locker>
void FilePersistenceManager<Locker>::open(const string& path)
{
    if ((fd_ = ::open(path.c_str(), O_RDWR)) < 0)
    {
        throw Exception(util::format("Trying to open file '{}'", path), 
            Error::FILEOPEN);
    }
    passFdToLocker_();
}


template <class Locker>
void FilePersistenceManager<Locker>::close()
{
    locker_.setFile(EMPTY_FD);
    ::close(fd_);
    fd_ = EMPTY_FD;
    
}


template <class Locker>
void FilePersistenceManager<Locker>::read(page_number number, PageBuffer* page)
{
    moveToPage_(number);
    int code = 0;
    if ((code = ::read(fd_, page->buffer, PAGE_SIZE)) < PAGE_SIZE)
    {
        throw Exception(util::format("'{}'\nwhile trying to read page nr {}", 
            code, number), Error::READ);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::write(page_number number, 
    const PageBuffer* page)
{
    moveToPage_(number);
    if (::write(fd_, page->buffer, PAGE_SIZE) < PAGE_SIZE)
    {      
        throw Exception(util::format("Trying to write page nr {}", number), 
            Error::WRITE);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::ensurePages(page_number page)
{
    moveToPage_(page);
    PageBuffer buffer;
    write(page, &buffer);
}


template <class Locker>
void FilePersistenceManager<Locker>::moveToPage_(page_number page)
{
    if (::lseek(fd_, page * PAGE_SIZE, SEEK_SET) < 0)
    {
        throw Exception(util::format("Trying to move to page nr {}", page),
            Error::SEEK);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::passFdToLocker_()
{
    locker_.setFile(fd_);
}


template <class Locker>
typename FilePersistenceManager<Locker>::ReadLock
FilePersistenceManager<Locker>::readLock(page_number page)
{
    return locker_.readLock(page);
}


template <class Locker>
typename FilePersistenceManager<Locker>::WriteLock
FilePersistenceManager<Locker>::writeLock(page_number page)
{
    return locker_.writeLock(page);
}


}



#endif // __PAGANINI_PAGING_FILE_PERSISTENCE_MANAGER_H__
