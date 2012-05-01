/*
    Zestaw niskopoziomowych funkcji umozliwiajacy dostep do pliku z danymi.
*/
#ifndef __PAGING_PAGE_MANAGER_H__
#define __PAGING_PAGE_MANAGER_H__

#ifdef _PAGANINI
#   include "paging/page.h"
#else 
#   include <paganini/paging/page.h>
#endif

// Ilosc stron alokowanych poczatkowo (co najmniej 2 - naglowek + pierwsza UV)
#define INITIAL_SIZE (2 + 0)

// Ilosc stron alokowana za kazdym razem, gdy brakuje miejsca
#define GROWTH_RATE 20


// Tworzy nowy plik bazy danych - strone naglowka, i pierwsza strone UV.
int pdbCreateDatabaseFile(const char* path);

// Inicjalizuje managera stronnicowania przy uzyciu istniejacego pliku
int pdbPageManagerStart(const char* path);

// Konczy dzialanie managera stronnicowania
int pdbPageManagerStop(void);

// Zwraca numer wolnej strony. W razie potrzeby zwieksza rozmiar pliku, i
// potencjalnie tworzy potrzebne strony UV. Strona jest zaznaczona jako uzywana.
pdbPageNumber pdbAllocPage(void);

// Zwalnia strone o podanym numerze. Strona jest zaznaczana jako wolna, moze
// zostac ponownie uzyta.
int pdbDeletePage(pdbPageNumber number);

// Wczytuje do podanego bufora strone o zadanym numerze. 
int pdbReadPage(pdbPageNumber number, pdbPageBuffer buffer);

// Zapisuje do strony o podanym numerze dane z bufora. W celu zachowania 
// spojnosci danych, strona powinna istniec i byc zaznaczona jako uzyta.
int pdbWritePage(pdbPageNumber number, pdbPageBuffer buffer);


#endif // __PAGING_PAGE_MANAGER_H__

