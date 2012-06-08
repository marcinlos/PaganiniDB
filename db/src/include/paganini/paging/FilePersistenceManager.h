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
#include <paganini/paging/Page.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/util/format.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
using std::string;


namespace paganini
{


template <class Locker>
class FilePersistenceManager
{
public:
    FilePersistenceManager(const Locker& locker = Locker());
    
    // Tworzy nowy plik w podanej lokacji
    void create(const string& path);

    // Otwiera do czytania i pisania plik z podanej lokacji
    void open(const string& path);

    // Konczy dzialanie managera stronnicowania
    void close();
    
    // Zapewnia istnienie stron o numerach nie wiekszych niz podana
    void ensurePages(page_number number);
    
    // Wczytuje do podanego bufora dane ze strony o podanym numerze
    void read(page_number number, Page* page);
    
    // Zapisuje dane strony
    void write(page_number number, const Page* page);
    
    // Typy lockow
    typedef typename Locker::ReadLock ReadLock;
    typedef typename Locker::WriteLock WriteLock;
    
    // Tworzy lock czytelnika na podana strone
    ReadLock readLock(page_number page);
    
    // Tworzy lock pisarza na podana strone
    WriteLock writeLock(page_number page);
    
private:
    void moveToPage_(page_number page);
    
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
void FilePersistenceManager<Locker>::create(const string& path)
{
    // Tworzymy nowy plik dostepny dla uzytkownika
    if ((fd_ = ::open(path.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR)) < 0)
    {
        throw Exception(util::format("Trying to create file '{}'", path), 
            Error::FILECREATE);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::open(const string& path)
{
    if ((fd_ = ::open(path.c_str(), O_RDWR)) < 0)
    {
        throw Exception(util::format("Trying to open file '{}'", path), 
            Error::FILEOPEN);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::close()
{
    ::close(fd_);
    fd_ = EMPTY_FD;
}



template <class Locker>
void FilePersistenceManager<Locker>::read(page_number number, Page* page)
{
    moveToPage_(number);
    if (::read(fd_, page->buffer(), PAGE_SIZE) < PAGE_SIZE)
    {
        throw Exception(util::format("Trying to read page nr {}", number), 
            Error::READ);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::write(page_number number, 
    const Page* page)
{
    moveToPage_(number);
    if (::write(fd_, page->buffer(), PAGE_SIZE) < PAGE_SIZE)
    {      
        throw Exception(util::format("Trying to write page nr {}", number), 
            Error::WRITE);
    }
}


template <class Locker>
void FilePersistenceManager<Locker>::ensurePages(page_number page)
{
    moveToPage_(page);
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
