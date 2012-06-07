/*
    Klasa odpowiedzialna za fizyczna serializacje wiersza. Obecnie stosowany
    jest nastepujacy format:
    
    Ile      Znaczenie
     2       Flagi wiersza   
     2       Ilosc kolumn
  ceil(N/8)  Bitmapa NULL-i
     2       Laczna dlugosci danych pol stalej wielkosci
     n       Dane stalej dlugosci
     2       Ilosc kolumn o zmiennej dlugosci
2x (zmienne+1)  Offsety pol zmiennej dlugosci + offset za koniec
     n       Pola zmiennej dlugosci     
*/
#ifndef __PAGANINI_ROW_ROW_WRITER_H__
#define __PAGANINI_ROW_ROW_WRITER_H__

#include <paganini/row/Row.h>
#include <paganini/util/bits.h>
#include <paganini/row/FieldFactory.h>


namespace paganini
{


class RowWriter
{
public:
    RowWriter();
    
    // Zapisuje podany wiersz do miejsca w pamieci wyznaczonego przez argument
    // buffer. Zwraca ilosc zapisanych bajtow.
    size16 write(raw_data buffer, const Row& row);
    
    // Oblicza ilosc miejsca potrzebna do zapisania podanego wiersza
    size16 length(const Row& row) const;
    
private:
    FieldFactory& factory;
    
    // Tworzy bitmape NULL-i w wierszu
    util::Bitmap createNullBitmap(const Row& row) const;
    
    // Oblicza sumaryczny rozmiar pol stalej wielkosci.
    size16 totalFixedSize(const RowFormat& format) const;
};


}


#endif // __PAGANINI_ROW_ROW_WRITER_H__
