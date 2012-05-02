/*
    Zestaw niskopoziomowych funkcji umozliwiajacy dostep do pliku z danymi.
*/
#ifndef __PAGING_PAGE_MANAGER_H__
#define __PAGING_PAGE_MANAGER_H__

#ifdef _PAGANINI
#   include "paging/Page.h"
#else 
#   include <paganini/paging/Page.h>
#endif

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


// Tworzy nowy plik bazy danych - strone naglowka, i pierwsza strone UV.
int pdbCreateDatabaseFile(const char* path);

// Inicjalizuje managera stronnicowania przy uzyciu istniejacego pliku
int pdbPageManagerStart(const char* path);

// Konczy dzialanie managera stronnicowania
int pdbPageManagerStop();

// Zwraca numer wolnej strony. W razie potrzeby zwieksza rozmiar pliku, i
// potencjalnie tworzy potrzebne strony UV. Strona jest zaznaczona jako uzywana.
page_number pdbAllocPage();

// Zwalnia strone o podanym numerze. Strona jest zaznaczana jako wolna, moze
// zostac ponownie uzyta.
int pdbDeletePage(page_number number);

// Wczytuje do podanego bufora strone o zadanym numerze. 
int pdbReadPage(page_number number, Page* buffer);

// Zapisuje do strony o podanym numerze dane z bufora. W celu zachowania 
// spojnosci danych, strona powinna istniec i byc zaznaczona jako uzyta.
int pdbWritePage(page_number number, const Page* buffer);

}

#endif // __PAGING_PAGE_MANAGER_H__

