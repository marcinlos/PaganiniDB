#include "config.h"
#include <paganini/paging/Page.h>
#include <paganini/paging/PageManager.h>
#include <paganini/paging/DatabaseHeader.h>
#include <paganini/Error.h>
#include <paganini/util/format.h>
#include <paganini/util/bits.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <sstream>
using std::string;
using paganini::util::format;


namespace paganini
{


PageManager::PageManager()
{
}


// Ustawia pozycje wskaznika pliku na strone o podanym numerze
void PageManager::moveToPage_(page_number page)
{
    if (lseek(fd_, page * PAGE_SIZE, SEEK_SET) < 0)
    {
        throw Exception(Error::SEEK);
    }
}


// Wypelnia naglowek pliku bazy danych (ilosc stron i czas)
void PageManager::createHeader_()
{
    Page page(0, PageType::HEADER);

    // Zapisujemy metadane 
    DatabaseHeader* data = page.create<DatabaseHeader>("Default DB Name",
        FIRST_ALLOC);
        
    writePage(0, &page);
}


// Tworzy nowa strone UV, nastepna w stosunku do podanej.
page_number PageManager::createUVPage_(page_number previous_uv)
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
        page.header().next = new_page;
        writePage(previous_uv, &page);
    }
    
    // Wypelniamy naglowek strony, typ = UV_PAGE
    page.header().fill(new_page, PageType::UV);
    page.header().prev = previous_uv;

    page.clearData();
    
    writePage(new_page, &page);
    return new_page;
}


void PageManager::createFile(const string& path)
{
    // Tworzymy nowy plik dostepny dla uzytkownika
    if ((fd_ = open(path.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR)) < 0)
    {
        throw Exception(format("Trying to create file '{}'", path), 
            Error::FILECREATE);
    }
    // Alokujemy miejsce na pierwsze bloki
    moveToPage_(FIRST_ALLOC);
    
    // Tworzymy naglowek i pierwsza strone UV
    createHeader_();
    createUVPage_(NULL_PAGE);
    
    // Zamykamy plik - tu tylko tworzymy
    closeFile();
}


void PageManager::openFile(const string& path)
{
    if ((fd_ = open(path.c_str(), O_RDWR)) < 0)
    {
        throw Exception(format("Trying to open file '{}'", path), 
            Error::FILEOPEN);
    }
}


void PageManager::closeFile()
{
    close(fd_);
    fd_ = EMPTY_FD;
}


// Zwraca numer strony UV zawierajacej informacje o uzytkowaniu strony o
// podanym numerze. Zwraca UV w dwoch przypadkach: gdy podana zostaje strona
// UV, lub naglowek pliku.
page_number PageManager::findUV_(page_number number)
{
    page_number diff = (number - 1) % (PAGES_PER_UV + 1);
    
    // Naglowek pliku oraz strony UV nie odpowiadaja zadnym stronom UV
    if (number == HEADER_PAGE_NUMBER || diff == 0)
        return NULL_PAGE;
    else 
        return number - diff;
}


// Wczytuje do bufora strone UV zawierajaca informacje o stronie podanej jako 
// pierwszy argument. Zwraca numer strony UV, lub NULL_PAGE w przypadku,
// gdy strona nie posiada odpowiadajacej strony UV.
page_number PageManager::readUVOfPage_(page_number number, Page* page)
{
    page_number uv = findUV_(number);
    if (uv != NULL_PAGE)
    {
        readPage(uv, page);
    }       
    return uv;
}


// Zapisuje w strukturach wewnetrznych (strona UV) informacje, ze podana strona
// jest uzywana.
bool PageManager::markAsUsed_(page_number number)
{
    // Wczytujemy odpowiednia strone UV
    Page page;
    page_number uv;
    if ((uv = readUVOfPage_(number, &page)) == NULL_PAGE)
        return false;
        
    // Numer bitu to pozycja wzgledem strony UV
    int bit = number - (uv + 1);
    util::set_bit(page.data(), bit);
    
    writePage(uv, &page);
    return true;
}


// Zapisuje w strukturach wewnetrznych informacje, ze podana strona jest wolna.
// Implementacja analogiczna do markAsUsed().
bool PageManager::markAsFree_(page_number number)
{
    Page page;
    page_number uv;
    if ((uv = readUVOfPage_(number, &page)) == NULL_PAGE)
        return false;
 
    int bit = number - (uv + 1);
    util::unset_bit(page.data(), bit);
 
    writePage(uv, &page);
    return true;
}


// W sekcji danych strony przechowywanej w podanym buforze szuka pierwszego
// zerowego bitu (nieuzywany blok). Zwraca jego pozycje, badz -1 gdy takowego 
// nie ma.
int PageManager::scanForFree_(const Page* uv)
{
    // Po bajtach... mozna by optymalniej, ale... oj tam
    for (int i = 0; i < PAGES_PER_UV / 8; ++ i)
    {
        // Napisalem funkcje do znajdowania NIEzerowego bitu...
        unsigned char b = ~uv->data()[i];
        if (b != 0)  
        {
            return i * 8 + util::first_nonzero_bit(b);
        }
    }
    return -1;
}


// Zwieksza plik o page_count stron ogolnego uzytku (+ strony UV).
void PageManager::growFile_(size32 page_count)
{
    // Wczytujemy z naglowka bazy danych informacje o ilosci wszystkich stron.
    // Moze daloby sie tego uniknac?
    Page page;
    readPage(HEADER_PAGE_NUMBER, &page);
        
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
    moveToPage_(header->page_count);  
    writePage(HEADER_PAGE_NUMBER, &page);
    
    // Tworzymy w razie potrzeby nowe UV-strony
    for (int i = 0; i < new_uv_count; ++ i)
    {
        last_uv = createUVPage_(last_uv);
    }   
}


// Znajduje wolna strone. Jesli jej nie ma, zwieksza plik. Zwraca jej numer.
page_number PageManager::findFree_()
{
    // Pobieramy ilosc stron
    Page page;
    readPage(HEADER_PAGE_NUMBER, &page);
    DatabaseHeader* db_header = page.get<DatabaseHeader>();
    size32 count = db_header->page_count;
    
    page_number uv = FIRST_UV_PAGE_NUMBER;
    page_number prev = uv;
    
    int partial = 0;
    
    // Przechodzimy po liscie stron UV, szukajac wolnej strony
    while (uv != NULL_PAGE)
    {
        readPage(uv, &page);
        int num = scanForFree_(&page);
        if (num != -1)
        {
            page_number free = page.header().number + num + 1;

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
        uv = page.header().next;
    }
    // Nie bylo wolnego, trzeba zaalokowac nowe strony
    growFile_(GROWTH_RATE);

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
        readPage(prev, &page);
        return page.header().next + 1;
    }
}


page_number PageManager::allocPage()
{
    page_number free = findFree_();

    // Wypelniamy obowiazki administracyjne
    markAsUsed_(free);
    Page page(free);
 
    writePage(free, &page);
    return free;
}


bool PageManager::deletePage(page_number number)
{
    return markAsFree_(number);
}


void PageManager::readPage(page_number number, Page* page)
{
    moveToPage_(number);
    if (read(fd_, page->buffer(), PAGE_SIZE) < PAGE_SIZE)
    {
        throw Exception(format("Trying to read page nr {}", number), 
            Error::READ);
    }
}


void PageManager::writePage(page_number number, const Page* page)
{
    moveToPage_(number);
    if (write(fd_, page->buffer(), PAGE_SIZE) < PAGE_SIZE)
    {      
        throw Exception(format("Trying to write page nr {}", number), 
            Error::WRITE);
    }
}



}

