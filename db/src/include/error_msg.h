#ifndef _ERROR_MSG_H
#define _ERROR_MSG_H

/* 
    Krytyczny blad systemowy, wypisuje podany tekst, komunikat o bledzie
    na podstawie wartosci errno i wychodzi z programu poprzez exit(1)
*/
void fatal_sys(const char* text, ...);

/*
    Blad systemowy nie powodujacy zakonczenia dzialania programu.
*/
void error_sys(const char* text, ...);

/*
    Krytyczny blad niesystemowy (nie wypisuje wartosci errno).
*/
void fatal_usr(const char* text, ...);

/*
    Blad niesystemowy nie powodujacy zakonczenia dzialania programu.
*/
void error_usr(const char* text, ...);

/*
    Krytyczny blad bazy danych
*/
void fatal_pdb(const char* text, ...);

/*
    Blad bazy danych nie powodujacy zakonczenia dzialania programu.
*/
void error_pdb(const char* text, ...);

#endif /* ! _ERROR_MSG_H */
