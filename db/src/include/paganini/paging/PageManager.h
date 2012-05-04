/*
   Klasa udostepniajaca niskopoziomowy (oparty na stronach) dostep do
   pliku z danymi.
*/
#ifndef __PAGANINI_PAGING_PAGE_MANAGER_H__
#define __PAGANINI_PAGING_PAGE_MANAGER_H__

#include <paganini/paging/Page.h>


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

// Manager stronnicowania, odpowiedzialny za tworzenie fizycznych plikow
// bazy danych, jak rowniez zapis/odczyt stron.
class PageManager
{
private:
    const static int EMPTY_FD = -1;
    int fd;
    void moveToPage(page_number page);
    void createHeader();
    page_number createUVPage(page_number previous_uv);
    static page_number findUV(page_number number);
    page_number readUVOfPage(page_number number, Page* page);
    bool markAsUsed(page_number number);
    bool markAsFree(page_number number);
    int scanForFree(const Page* uv);
    void growFile(size32 page_count);
    page_number findFree();

public:
    // Tworzy nowy plik bazy danych - strone naglowka, i pierwsza strone UV.
    void createFile(const char* path);

    // Inicjalizuje managera stronnicowania przy uzyciu istniejacego pliku
    void openFile(const char* path);

    // Konczy dzialanie managera stronnicowania
    void closeFile();

    // Zwraca numer wolnej strony. W razie potrzeby zwieksza rozmiar pliku, i
    // potencjalnie tworzy potrzebne strony UV. Strona jest zaznaczona jako uzywana.
    page_number allocPage();

    // Zwalnia strone o podanym numerze. Strona jest zaznaczana jako wolna, moze
    // zostac ponownie uzyta.
    bool deletePage(page_number number);

    // Wczytuje do podanego bufora strone o zadanym numerze. 
    void readPage(page_number number, Page* buffer);

    // Zapisuje do strony o podanym numerze dane z bufora. W celu zachowania 
    // spojnosci danych, strona powinna istniec i byc zaznaczona jako uzyta.
    void writePage(page_number number, const Page* buffer);

};

}

#endif // __PAGANINI_PAGING_PAGE_MANAGER_H__
