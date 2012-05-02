#include "config.h"
#include "paging/Page.h"
#include "paging/PageManager.h"
#include "paging/DatabaseHeader.h"

#include "util/bits.h"
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include "pdb_error.h"
using std::string;


namespace paganini
{



// Ustawia pozycje wskaznika pliku na strone o podanym numerze
int PageManager::moveToPage(page_number page)
{
    if (lseek(fd, page * PAGE_SIZE, SEEK_SET) < 0)
    {
        _pdbSetErrno(Error::SEEK);
        return -1;
    }
    return 0;
}


// Wypelnia naglowek pliku bazy danych (ilosc stron i czas)
int PageManager::createHeader()
{
    Page page(0, PageType::HEADER);

    // Zapisujemy metadane 
    DatabaseHeader* data = page.create<DatabaseHeader>("Default DB Name",
        FIRST_ALLOC);
        
    if (writePage(0, &page) < 0)
        return -1;
    return 0;
}


// Tworzy nowa strone UV, nastepna w stosunku do podanej.
int PageManager::createUVPage(page_number previous_uv)
{
    // Obliczamy pozycje nowej strony UV - pierwsza jest szczegolnym przypadkiem
    page_number new_page;
    if (previous_uv == NULL_PAGE)
        new_page = FIRST_UV_PAGE_NUMBER;
    else
        new_page = previous_uv + PAGES_PER_UV + 1;
    
    // Uaktualniamy nexta w poprzedniej UV-stronie, jesli istnieje
    Page page;
    
    if (previous_uv != NULL_PAGE)
    {
        readPage(previous_uv, &page);
        page.header.next = new_page;
        if (writePage(previous_uv, &page) < 0)
            return NULL_PAGE;
    }
    
    // Wypelniamy naglowek strony, typ = UV_PAGE
    page.header.fill(new_page, PageType::UV);
    page.header.prev = previous_uv;

    page.clearData();
    
    if (writePage(new_page, &page) < 0)
        return NULL_PAGE;
    return new_page;
}


int PageManager::createFile(const char* path)
{
    // Tworzymy nowy plik dostepny dla uzytkownika
    if ((fd = open(path, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR)) < 0)
    {
        _pdbSetErrno(Error::FILECREATE);
        return -1;
    }
    // Alokujemy miejsce na pierwsze bloki
    moveToPage(FIRST_ALLOC);
    
    // Tworzymy naglowek i pierwsza strone UV
    createHeader();
    createUVPage(NULL_PAGE);
    
    // Zamykamy plik - tu tylko tworzymy
    close(fd);
    fd = -1;
}


int PageManager::openFile(const char* path)
{
    if ((fd = open(path, O_RDWR)) < 0)
    {
        _pdbSetErrno(Error::FILEOPEN);
        return -1;
    }
    return 0;
}


int PageManager::closeFile()
{
    close(fd);
    fd = -1;
    return 0;
}


// Zwraca numer strony UV zawierajacej informacje o uzytkowaniu strony o
// podanym numerze. Zwraca UV w dwoch przypadkach: gdy podana zostaje strona
// UV, lub naglowek pliku.
page_number PageManager::findUV(page_number number)
{
    page_number diff = (number - 1) % (PAGES_PER_UV + 1);
    
    // Naglowek pliku oraz strony UV nie odpowiadaja zadnym stronom UV
    if (number == HEADER_PAGE_NUMBER || diff == 0)
        return NULL_PAGE;
    else 
        return number - diff;
}





// Wczytuje do bufora strone UV zawierajaca informacje o stronie podanej jako 
// pierwszy argument
page_number PageManager::readUVOfPage(page_number number, Page* page)
{
    page_number uv = findUV(number);
    if (uv != NULL_PAGE)
    {
        if (readPage(uv, page) < 0)
            return NULL_PAGE;
    }       
    return uv;
}


// Zapisuje w strukturach wewnetrznych (strona UV) informacje, ze podana strona
// jest uzywana.
int PageManager::markAsUsed(page_number number)
{
    // Wczytujemy odpowiednia strone UV
    Page page;
    page_number uv;
    if ((uv = readUVOfPage(number, &page)) == NULL_PAGE)
        return -1;
        
    // Numer bitu to pozycja wzgledem strony UV
    int bit = number - (uv + 1);
    util::set_bit(page.data, bit);
    
    if (writePage(uv, &page) < 0)
        return -1;
    return 0;
}


// Zapisuje w strukturach wewnetrznych informacje, ze podana strona jest wolna.
// Implementacja analogiczna do markAsUsed().
int PageManager::markAsFree(page_number number)
{
    Page page;
    page_number uv;
    if ((uv = readUVOfPage(number, &page)) == NULL_PAGE)
        return -1;
 
    int bit = number - (uv + 1);
    util::unset_bit(page.data, bit);
 
    if (writePage(uv, &page) < 0)
        return -1;
    return 0;
}





// W sekcji danych strony przechowywanej w podanym buforze szuka pierwszego
// zerowego bitu (nieuzywany blok). Zwraca jego pozycje, badz -1 gdy takowego 
// nie ma.
int PageManager::scanForFree(const Page* uv)
{
    // Po bajtach... mozna by optymalniej, ale... oj tam
    for (int i = 0; i < PAGES_PER_UV / 8; ++ i)
    {
        // Napisalem funkcje do znajdowania NIEzerowego bitu...
        unsigned char b = ~uv->data[i];
        if (b != 0)  
        {
            return i * 8 + util::first_nonzero_bit(b);
        }
    }
    return -1;
}


// Zwieksza plik o page_count stron ogolnego uzytku (+ strony UV).
int PageManager::growFile(size32 page_count)
{
    // Wczytujemy z naglowka bazy danych informacje o ilosci wszystkich stron.
    // Moze daloby sie tego uniknac?
    Page page;
    if (readPage(HEADER_PAGE_NUMBER, &page) < 0)
        return -1;
        
    DatabaseHeader* header = page.get<DatabaseHeader>();
    int count = header->page_count;
    int diff = (count - 2) % (PAGES_PER_UV + 1);
    // Pozycja ostatniej strony UV przyda sie do tworzenia kolejnych stron UV
    size32 last_uv = (count - 1) - diff;
    
    // Obliczamy ilosc stron UV, ktore nalezy dodac:
    // - nonuv_count to sumaryczna ilosc stron obslugiwanych tymczasowo przez 
    //   ostatnia strone UV. Jest to suma tych juz obslugiwanych, oraz nowych.
    // - new_uv_count to ilosc potrzebnych nowych stron UV i wynosi
    //     ceil(nonuv_ount / PAGES_PER_UV) - 1
    int nonuv_count = page_count + diff;
    int new_uv_count = (nonuv_count - 1) / PAGES_PER_UV;
    int total = new_uv_count + page_count;

    // Fizycznie zwiekszamy plik i uaktualniamy naglowek    
    header->page_count += total;
    if (moveToPage(header->page_count) < 0)
        return -1;
    
    if (writePage(HEADER_PAGE_NUMBER, &page) < 0)
        return -1;
    
    // Tworzymy w razie potrzeby nowe UV-strony
    for (int i = 0; i < new_uv_count; ++ i)
    {
        if ((last_uv = createUVPage(last_uv)) == NULL_PAGE)
            return -1;
    }   
    return 0;
}


// Znajduje wolna strone. Jesli jej nie ma, zwieksza plik. Zwraca jej numer,
// lub NULL_PAGE w przypadku bledu.
page_number PageManager::findFree()
{
    // Pobieramy ilosc stron
    Page page;
    if (readPage(HEADER_PAGE_NUMBER, &page) < 0)
        return -1;
    DatabaseHeader* db_header = page.get<DatabaseHeader>();
    size32 count = db_header->page_count;
    
    page_number uv = FIRST_UV_PAGE_NUMBER;
    page_number prev = uv;
    
    int partial = 0;
    
    // Przechodzimy po liscie stron UV, szukajac wolnej strony
    while (uv != NULL_PAGE)
    {
        if (readPage(uv, &page) < 0)
            return -1;
        int num = scanForFree(&page);
        if (num != -1)
        {
            page_number free = page.header.number + num + 1;

            // Sprawdzenie dla niepelnych stron UV
            if (free < count)
                return free;
            else
            {
                partial = free;
                break;
            }
        }
        prev = uv;
        uv = page.header.next;
    }
    // Nie bylo wolnego, trzeba zaalokowac nowe strony
    if (growFile(GROWTH_RATE) < 0)
        return -1;
    // Jesli zbior stron opisywany przez ostatnia strone UV nie byl pelny,
    // to znaleziona wowczas wolna strona teraz jest juz poprawna.
    if (partial != 0)
    {
        return partial;
    }
    // Jesli nie, to zaalokowano nowa/e strony UV, mozna wziac pierwsza strone
    // za pierwsza zaalokowana strona UV
    else
    {
        if (readPage(prev, &page) < 0)
            return NULL_PAGE;
        return page.header.next + 1;
    }
}


page_number PageManager::allocPage()
{
    page_number free;
    if ((free = findFree()) == NULL_PAGE)
        return NULL_PAGE;

    // Wypelniamy obowiazki administracyjne
    markAsUsed(free);
    Page page(free);
 
    if (writePage(free, &page) < 0)
        return NULL_PAGE;
    return free;
}


int PageManager::deletePage(page_number number)
{
    return markAsFree(number);
}


int PageManager::readPage(page_number number, Page* buffer)
{
    moveToPage(number);
    if (read(fd, buffer, PAGE_SIZE) < PAGE_SIZE)
    {
        _pdbSetErrno(Error::READ);
        return -1;
    }
    return 0;
}


int PageManager::writePage(page_number number, const Page* buffer)
{
    moveToPage(number);
    if (write(fd, buffer, PAGE_SIZE) < PAGE_SIZE)
    {
        _pdbSetErrno(Error::WRITE);
        return -1;
    }
    return 0;
}



}

