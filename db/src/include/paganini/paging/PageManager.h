/*
   Klasa udostepniajaca niskopoziomowy (oparty na stronach) dostep do
   pliku z danymi.
*/
#ifndef __PAGANINI_PAGING_PAGE_MANAGER_H__
#define __PAGANINI_PAGING_PAGE_MANAGER_H__

#include <paganini/paging/PageBuffer.h>
#include <paganini/paging/DatabaseHeader.h>
#include <paganini/Error.h>
#include <paganini/util/format.h>
#include <paganini/util/bits.h>
#include <string>
#include <memory>
using std::string;


namespace paganini
{

// Ilosc stron poczatkowych, nie uzywanych jako UV ani na dane. Zapewne
// tylko naglowek - 1
static const size32 METADATA_PAGES = 1;

// Ilosc stron alokowanych poczatkowo poza danymi pliku (METADATA_PAGES)
// (co najmniej 1 - pierwsza strona UV)
static const size32 INITIAL_PAGES = 1;

// Sumaryczna ilosc stron alokowana na poczatku (metadane + ilosc poczatkowa
// stron dodatkowych)
static const size32 FIRST_ALLOC = METADATA_PAGES + INITIAL_PAGES;

// Ilosc stron alokowana za kazdym razem, gdy brakuje miejsca
static const size32 GROWTH_RATE = 20;


// Specjalne strony
static const page_number HEADER_PAGE_NUMBER = 0;
static const page_number FIRST_UV_PAGE_NUMBER = 1;

// Sprawdza, czy strona o podanym numerze to strona UV
inline bool isUV(page_number page)
{
    return (page - METADATA_PAGES) % (PAGES_PER_UV + 1) == 0;
}

// Manager stronnicowania, odpowiedzialny za zarzadzanie logiczna struktura
// systemu ston. Parametryzowany klasa PersistenceManager, ktorej zadaniem
// jest zapewnienie fizycznego dostepu do odczytu/zapisu danych, jak rowniez
// lockowania. Wydaje mi sie, ze jest na tyle polaczone z warstwa persystencji,
// ze nie ma sensu probowac ich rozdzielac w tym miejscu.
template <class PersistenceManager>
class PageManager: private PersistenceManager
{
public:
    PageManager();
    
    // Tworzy nowy plik bazy danych - strone naglowka, i pierwsza strone UV.
    void createFile(const string& path);

    // Inicjalizuje managera stronnicowania przy uzyciu istniejacego pliku
    void openFile(const string& path);

    // Konczy dzialanie managera stronnicowania
    void closeFile();

    // Zwraca numer wolnej strony. W razie potrzeby zwieksza rozmiar pliku, i
    // potencjalnie tworzy potrzebne strony UV. Strona jest zaznaczona jako 
    // uzywana.
    page_number allocPage();

    // Zwalnia strone o podanym numerze. Strona jest zaznaczana jako wolna, moze
    // zostac ponownie uzyta.
    bool deletePage(page_number number);

    // Wczytuje do podanego bufora strone o zadanym numerze. 
    void readPage(page_number number, PageBuffer* page);
    
    // Zwraca nowo zaalokowana strone z zawartoscia strony o podanym numerze
    std::unique_ptr<PageBuffer> readPage(page_number number);

    // Zapisuje do strony o podanym numerze dane z bufora. W celu zachowania 
    // spojnosci danych, strona powinna istniec i byc zaznaczona jako uzyta.
    void writePage(page_number number, const PageBuffer* page);
    
    // Typy lockow
    typedef typename PersistenceManager::ReadLock ReadLock;
    typedef typename PersistenceManager::WriteLock WriteLock;
    
    // Forwardujemy lockowanie bezposrednio do PersistenceManagera
    
    // Blokuje strone do czytania
    using PersistenceManager::readLock;
    
    // Blokuje strone do pisania
    using PersistenceManager::writeLock;
    
private:
    void createHeader_();
    page_number createUVPage_(page_number previous_uv);
    static page_number findUV_(page_number number);
    page_number readUVOfPage_(page_number number, PageBuffer* page);
    bool markAsUsed_(page_number number);
    bool markAsFree_(page_number number);
    int scanForFree_(const PageBuffer* uv);
    void growFile_(size32 page_count);
    page_number findFree_();
};


template <class PersistenceManager>
PageManager<PersistenceManager>::PageManager()
{
}


// Wypelnia naglowek pliku bazy danych (ilosc stron i czas)
template <class PersistenceManager>
void PageManager<PersistenceManager>::createHeader_()
{
    PageBuffer page(0, PageType::HEADER);

    // Zapisujemy metadane 
    page.create<DatabaseHeader>("Default DB Name", FIRST_ALLOC);
        
    writePage(0, &page);
}


// Tworzy nowa strone UV, nastepna w stosunku do podanej.
template <class PersistenceManager>
page_number PageManager<PersistenceManager>::createUVPage_(
    page_number previous_uv)
{
    // Obliczamy pozycje nowej strony UV - pierwsza jest szczegolnym przypadkiem
    page_number new_page;
    if (previous_uv == NULL_PAGE)
        new_page = FIRST_UV_PAGE_NUMBER;
    else
        new_page = previous_uv + PAGES_PER_UV + 1;
    
    // Uaktualniamy nexta w poprzedniej UV-stronie, jesli istnieje
    PageBuffer page;
    
    if (previous_uv != NULL_PAGE)
    {
        readPage(previous_uv, &page);
        page.header.next = new_page;
        writePage(previous_uv, &page);
    }
    
    // Wypelniamy naglowek strony, typ = UV_PAGE
    page.header.fill(new_page, PageType::UV);
    page.header.prev = previous_uv;

    page.clearData();
    
    writePage(new_page, &page);
    return new_page;
}


template <class PersistenceManager>
void PageManager<PersistenceManager>::createFile(const string& path)
{
    // Tworzymy nowy plik dostepny dla uzytkownika
    this->create(path);
    
    // Alokujemy miejsce na pierwsze bloki
    this->ensurePages(FIRST_ALLOC);
    
    // Tworzymy naglowek i pierwsza strone UV
    createHeader_();
    createUVPage_(NULL_PAGE);
    
    // Zamykamy plik - tu tylko tworzymy
    closeFile();
}


template <class PersistenceManager>
void PageManager<PersistenceManager>::openFile(const string& path)
{
    this->open(path);
}


template <class PersistenceManager>
void PageManager<PersistenceManager>::closeFile()
{
    this->close();
}


// Zwraca numer strony UV zawierajacej informacje o uzytkowaniu strony o
// podanym numerze. Zwraca UV w dwoch przypadkach: gdy podana zostaje strona
// UV, lub naglowek pliku.
template <class PersistenceManager>
page_number PageManager<PersistenceManager>::findUV_(page_number number)
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
template <class PersistenceManager>
page_number PageManager<PersistenceManager>::readUVOfPage_(page_number number, 
    PageBuffer* page)
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
template <class PersistenceManager>
bool PageManager<PersistenceManager>::markAsUsed_(page_number number)
{
    // Wczytujemy odpowiednia strone UV
    PageBuffer page;
    page_number uv;
    if ((uv = readUVOfPage_(number, &page)) == NULL_PAGE)
        return false;
        
    // Numer bitu to pozycja wzgledem strony UV
    int bit = number - (uv + 1);
    util::set_bit(page.data, bit);
    
    writePage(uv, &page);
    return true;
}


// Zapisuje w strukturach wewnetrznych informacje, ze podana strona jest wolna.
// Implementacja analogiczna do markAsUsed().
template <class PersistenceManager>
bool PageManager<PersistenceManager>::markAsFree_(page_number number)
{
    PageBuffer page;
    page_number uv;
    if ((uv = readUVOfPage_(number, &page)) == NULL_PAGE)
        return false;
 
    int bit = number - (uv + 1);
    util::unset_bit(page.data, bit);
 
    writePage(uv, &page);
    return true;
}


// W sekcji danych strony przechowywanej w podanym buforze szuka pierwszego
// zerowego bitu (nieuzywany blok). Zwraca jego pozycje, badz -1 gdy takowego 
// nie ma.
template <class PersistenceManager>
int PageManager<PersistenceManager>::scanForFree_(const PageBuffer* uv)
{
    // Po bajtach... mozna by optymalniej, ale... oj tam
    for (unsigned int i = 0; i < PAGES_PER_UV / 8; ++ i)
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
template <class PersistenceManager>
void PageManager<PersistenceManager>::growFile_(size32 page_count)
{
    // Wczytujemy z naglowka bazy danych informacje o ilosci wszystkich stron.
    // Moze daloby sie tego uniknac?
    PageBuffer page;
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
    this->ensurePages(header->page_count);  
    writePage(HEADER_PAGE_NUMBER, &page);
    
    // Tworzymy w razie potrzeby nowe UV-strony
    for (int i = 0; i < new_uv_count; ++ i)
    {
        last_uv = createUVPage_(last_uv);
    }   
}


// Znajduje wolna strone. Jesli jej nie ma, zwieksza plik. Zwraca jej numer.
template <class PersistenceManager>
page_number PageManager<PersistenceManager>::findFree_()
{
    // Pobieramy ilosc stron
    PageBuffer page;
    readPage(HEADER_PAGE_NUMBER, &page);
    DatabaseHeader* db_header = page.get<DatabaseHeader>();
    int count = db_header->page_count;
    
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
        return page.header.next + 1;
    }
}


template <class PersistenceManager>
page_number PageManager<PersistenceManager>::allocPage()
{
    page_number free = findFree_();

    // Wypelniamy obowiazki administracyjne
    markAsUsed_(free);
    PageBuffer page(free);
 
    writePage(free, &page);
    return free;
}


template <class PersistenceManager>
bool PageManager<PersistenceManager>::deletePage(page_number number)
{
    return markAsFree_(number);
}


template <class PersistenceManager>
void PageManager<PersistenceManager>::readPage(page_number number, 
    PageBuffer* page)
{
    this->read(number, page);
}


template <class PersistenceManager>
std::unique_ptr<PageBuffer> 
PageManager<PersistenceManager>::readPage(page_number number)
{
    std::unique_ptr<PageBuffer> page(new PageBuffer);
    readPage(number, page.get());
    return page;
}


template <class PersistenceManager>
void PageManager<PersistenceManager>::writePage(page_number number, 
    const PageBuffer* page)
{
    this->write(number, page);
}


}

#endif // __PAGANINI_PAGING_PAGE_MANAGER_H__

