#include "config.h"
#include "paging/page.h"
#include "paging/page_manager.h"
#include "paging/dbfile_header.h"
#include "pdb_error.h"
#include <fcntl.h>
#include <string.h>
#include <time.h>



// Deskryptor pliku bazy danych
static int fd = -1; 


// Ustawia pozycje wskaznika pliku na strone o podanym numerze
static int _pdbMoveToPage(pdbPageNumber page)
{
    if (lseek(fd, page * PAGE_SIZE, SEEK_SET) < 0)
    {
        _pdbSetErrno(PDBE_SEEK);
        return -1;
    }
    return 0;
}


// Wypelnia naglowek pliku bazy danych (ilosc stron i czas)
static int _pdbCreateHeader(void)
{
    pdbPageBuffer buffer;
    
    // Wypelniamy naglowek
    pdbPageHeader* header = (pdbPageHeader*) buffer;
    pdbFillHeader(header, 0);
    SET_PAGE_TYPE(header->flags, HEADER_PAGE);

    // Zapisujemy metadane 
    pdbDatabaseHeader* data = (pdbDatabaseHeader*) (buffer + DATA_OFFSET);
    data->page_count = INITIAL_SIZE;
    data->creation_time = time(NULL);
    strcpy(data->db_name, "Default DB Name");
    
    if (pdbWritePage(0, buffer) < 0)
        return -1;
    return 0;
}


// Tworzy nowa strone UV, nastepna w stosunku do podanej.
static int _pdbCreateUVPage(pdbPageNumber previous_uv)
{
    // Obliczamy pozycje nowej strony UV - pierwsza jest szczegolnym przypadkiem
    pdbPageNumber new_page;
    if (previous_uv == NULL_PAGE)
        new_page = FIRST_UV_PAGE_NUMBER;
    else
        new_page = previous_uv + PAGES_PER_UV + 1;
    
    // Uaktualniamy nexta w poprzedniej UV-stronie, jesli istnieje
    pdbPageBuffer buffer;
    pdbPageHeader* header = (pdbPageHeader*) buffer;
    
    if (previous_uv != NULL_PAGE)
    {
        pdbReadPage(previous_uv, buffer);
        header->next = new_page;
        if (pdbWritePage(previous_uv, buffer) < 0)
            return NULL_PAGE;
    }
    
    // Wypelniamy naglowek strony, typ = UV_PAGE
    pdbFillHeader(header, new_page);
    header->prev = previous_uv;
    SET_PAGE_TYPE(header->flags, UV_PAGE);
    pdbData data = buffer + DATA_OFFSET;
    memset(data, 0, DATA_SIZE);
    
    if (pdbWritePage(new_page, buffer) < 0)
        return NULL_PAGE;
    return new_page;
}


int pdbCreateDatabaseFile(const char* path)
{
    // Tworzymy nowy plik dostepny dla uzytkownika
    if ((fd = open(path, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR)) < 0)
    {
        _pdbSetErrno(PDBE_FILECREATE);
        return -1;
    }
    // Alokujemy miejsce na pierwsze bloki
    _pdbMoveToPage(INITIAL_SIZE);
    
    // Tworzymy naglowek i pierwsza strone UV
    _pdbCreateHeader();
    _pdbCreateUVPage(NULL_PAGE);
    
    // Zamykamy plik - tu tylko tworzymy
    close(fd);
    fd = -1;
}


int pdbPageManagerStart(const char* path)
{
    if ((fd = open(path, O_RDWR)) < 0)
    {
        _pdbSetErrno(PDBE_FILEOPEN);
        return -1;
    }
    return 0;
}


int pdbPageManagerStop(void)
{
    close(fd);
    fd = -1;
    return 0;
}


// Zwraca numer strony UV zawierajacej informacje o uzytkowaniu strony o
// podanym numerze. Zwraca UV w dwoch przypadkach: gdy podana zostaje strona
// UV, lub naglowek pliku.
static pdbPageNumber _pdbFindUV(pdbPageNumber number)
{
    pdbPageNumber diff = (number - 1) % (PAGES_PER_UV + 1);
    
    // Naglowek pliku oraz strony UV nie odpowiadaja zadnym stronom UV
    if (number == HEADER_PAGE_NUMBER || diff == 0)
        return NULL_PAGE;
    else 
        return number - diff;
}


// Ustawia w data bit o podanym numerze
static void _pdbSetBit(pdbData data, int bit)
{
    data += bit / 8;
    *data |= (1 << (bit % 8));
}


// Zeruje w data bit o podanym numerze
static void _pdbUnsetBit(pdbData data, int bit)
{
    data += bit / 8;
    *data &= ~(1 << (bit % 8));
}


// Wczytuje do bufora strone UV zawierajaca informacje o stronie podanej jako 
// pierwszy argument
static pdbPageNumber _pdbReadUVOfPage(pdbPageNumber number, 
    pdbPageBuffer buffer)
{
    pdbPageNumber uv = _pdbFindUV(number);
    if (uv != NULL_PAGE)
    {
        if (pdbReadPage(uv, buffer) < 0)
            return NULL_PAGE;
    }       
    return uv;
}


// Zapisuje w strukturach wewnetrznych (strona UV) informacje, ze podana strona
// jest uzywana.
static int _pdbMarkAsUsed(pdbPageNumber number)
{
    // Wczytujemy odpowiednia strone UV
    pdbPageBuffer buffer;
    pdbPageNumber uv;
    if ((uv = _pdbReadUVOfPage(number, buffer)) == NULL_PAGE)
        return -1;
        
    // Numer bitu to pozycja wzgledem strony UV
    int bit = number - (uv + 1);
    _pdbSetBit(buffer + DATA_OFFSET, bit);
    
    if (pdbWritePage(uv, buffer) < 0)
        return -1;
    return 0;
}


// Zapisuje w strukturach wewnetrznych informacje, ze podana strona jest wolna.
// Implementacja analogiczna do _pdbMarkAsUsed().
static int _pdbMarkAsFree(pdbPageNumber number)
{
    pdbPageBuffer buffer;
    pdbPageNumber uv;
    if ((uv = _pdbReadUVOfPage(number, buffer)) == NULL_PAGE)
        return -1;
 
    int bit = number - (uv + 1);
    _pdbUnsetBit(buffer + DATA_OFFSET, bit);
 
    if (pdbWritePage(uv, buffer) < 0)
        return -1;
    return 0;
}


// Znajduje niezerowy bit w podanym bajcie. Jesli go nie ma, zwraca -1.
// Perfidne wyszukiwanie binarne... 
// Naprawde powinienem wywalic te funkcje do bitow do jakiejs biblioteki
// pomocniczej.
static int _pdbFindNonzeroBit(unsigned char byte)
{
    if (byte & 0x0f)
    {
        if (byte & 0x03)
        {
            if (byte & 0x01) return 0;
            else return 1;
        }
        else // byte & 0x0c
        {
            if (byte & 0x04) return 2;
            else return 3;
        }
    }
    else if (byte & 0xf0)
    {
        if (byte & 0x30)
        {
            if (byte & 0x10) return 4;
            else return 5;
        }
        else // byte & 0xc0
        {
            if (byte & 0x40) return 6;
            else return 7;
        }
    }
    else return -1;
}


// W sekcji danych strony przechowywanej w podanym buforze szuka pierwszego
// zerowego bitu (nieuzywany blok). Zwraca jego pozycje, badz -1 gdy takowego 
// nie ma.
static int _pdbScanForFree(pdbPageBuffer uv)
{
    pdbData d = uv + DATA_OFFSET;
    // Po bajtach... mozna by optymalniej, ale... oj tam
    int i;
    for (i = 0; i < PAGES_PER_UV / 8; ++ i)
    {
        // Napisalem funkcje do znajdowania NIEzerowego bitu...
        unsigned char b = ~d[i];
        if (b != 0)  
        {
            return i * 8 + _pdbFindNonzeroBit(b);
        }
    }
    return -1;
}


// Zwieksza plik o page_count stron ogolnego uzytku (+ strony UV).
static int _pdbGrowFile(pdbSize32 page_count)
{
    // Wczytujemy z naglowka bazy danych informacje o ilosci wszystkich stron.
    // Moze daloby sie tego uniknac?
    pdbPageBuffer buffer;
    if (pdbReadPage(HEADER_PAGE_NUMBER, buffer) < 0)
        return -1;
        
    pdbDatabaseHeader* header = (pdbDatabaseHeader*) (buffer + DATA_OFFSET);
    int count = header->page_count;
    int diff = (count - 2) % (PAGES_PER_UV + 1);
    // Pozycja ostatniej strony UV przyda sie do tworzenia kolejnych stron UV
    pdbSize32 last_uv = (count - 1) - diff;
    
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
    if (_pdbMoveToPage(header->page_count) < 0)
        return -1;
    
    if (pdbWritePage(HEADER_PAGE_NUMBER, buffer) < 0)
        return -1;
    
    // Tworzymy w razie potrzeby nowe UV-strony
    int i;
    for (i = 0; i < new_uv_count; ++ i)
    {
        if ((last_uv = _pdbCreateUVPage(last_uv)) == NULL_PAGE)
            return -1;
    }   
    return 0;
}


// Znajduje wolna strone. Jesli jej nie ma, zwieksza plik. Zwraca jej numer,
// lub NULL_PAGE w przypadku bledu.
static pdbPageNumber _pdbFindFree(void)
{
    // Pobieramy ilosc stron
    pdbPageBuffer buffer;
    if (pdbReadPage(HEADER_PAGE_NUMBER, buffer) < 0)
        return -1;
    pdbDatabaseHeader* db_header = (pdbDatabaseHeader*) (buffer + DATA_OFFSET);
    pdbSize32 count = db_header->page_count;
    
    pdbPageNumber page = FIRST_UV_PAGE_NUMBER;
    pdbPageHeader* header = (pdbPageHeader*) buffer;
    pdbPageNumber prev = page;
    
    int partial = 0;
    
    // Przechodzimy po liscie stron UV, szukajac wolnej strony
    while (page != NULL_PAGE)
    {
        if (pdbReadPage(page, buffer) < 0)
            return -1;
        int num = _pdbScanForFree(buffer);
        if (num != -1)
        {
            pdbPageNumber free = header->page_number + num + 1;

            // Sprawdzenie dla niepelnych stron UV
            if (free < count)
                return free;
            else
            {
                partial = free;
                break;
            }
        }
        prev = page;
        page = header->next;
    }
    // Nie bylo wolnego, trzeba zaalokowac nowe strony
    if (_pdbGrowFile(GROWTH_RATE) < 0)
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
        if (pdbReadPage(prev, buffer) < 0)
            return NULL_PAGE;
        return header->next + 1;
    }
}


pdbPageNumber pdbAllocPage(void)
{
    pdbPageNumber free;
    if ((free = _pdbFindFree()) == NULL_PAGE)
        return NULL_PAGE;

    // Wypelniamy obowiazki administracyjne
    _pdbMarkAsUsed(free);
    pdbPageBuffer buffer;
    pdbFillHeader(buffer, free);
 
    if (pdbWritePage(free, buffer) < 0)
        return NULL_PAGE;
    return free;
}


int pdbDeletePage(pdbPageNumber number)
{
    return _pdbMarkAsFree(number);
}


int pdbReadPage(pdbPageNumber number, pdbPageBuffer buffer)
{
    _pdbMoveToPage(number);
    if (read(fd, buffer, PAGE_SIZE) < PAGE_SIZE)
    {
        _pdbSetErrno(PDBE_READ);
        return -1;
    }
    return 0;
}


int pdbWritePage(pdbPageNumber number, pdbPageBuffer buffer)
{
    _pdbMoveToPage(number);
    if (write(fd, buffer, PAGE_SIZE) < PAGE_SIZE)
    {
        _pdbSetErrno(PDBE_WRITE);
        return -1;
    }
    return 0;
}



