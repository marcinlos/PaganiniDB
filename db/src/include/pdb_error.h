#ifndef __PDB_ERROR_H__
#define __PDB_ERROR_H__

// Funkcja zwracajaca tekstowy opis bledu
const char* pdbErrorMsg(int errno);

// Funkcja pobierajaca kod bledu
int pdbErrno();

// Funkcja ustawiajaca kod bledu - do uzytku wewnetrznego
int _pdbSetErrno(int errno);

// Wartosci kodow bledu
#define PDBE_NOERROR             0 // Brak bledu
#define PDBE_ARMAGEDON           1 // Test
#define PDBE_FILECREATE          2 // Nie udalo sie utworzyc pliku bazy danych
#define PDBE_FILEOPEN            3 // Nie udalo sie otworzyc pliku
#define PDBE_WRITE               4 // Blad zapisu
#define PDBE_READ                5 // Blad odczytu
#define PDBE_SEEK                6 // Blad pozycjonowania w pliku

#endif // __PDB_ERROR_H__
