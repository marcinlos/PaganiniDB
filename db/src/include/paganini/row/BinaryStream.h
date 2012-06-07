/*
    Klasa bazowa dla strumieni pozwalajacych w przenosny sposob
    zapisywac i odczytywac dane. Nie posiada wirtualnego destruktora,
    ta hierarchia klas nie powinna byc uzywana polimorficznie, ani
    rozszerzana.
*/
#ifndef __PAGANINI_ROW_BINARY_STREAM_H__
#define __PAGANINI_ROW_BINARY_STREAM_H__

#include <paganini/paging/types.h>


namespace paganini
{


class BinaryStream
{    
public:
    // Tworzy strumien operujacy na podanym buforze, z poczatkowym 
    // offsetem ustawionym na offset
    BinaryStream(raw_data buffer, size16 offset = 0);
       
    // Zwraca bierzaca pozycje czytania/pisania
    page_offset getOffset() const;
    
    // Ustawia bierzaca pozycje czytania/pisania
    void setOffset(page_offset offset);
    
    // Przesuwa obecna pozycje o offset bajtow
    void skip(page_offset offset);
    
    // Zwraca bufor, na ktorym strumien operuje
    raw_data getBuffer();
    
protected:
    raw_data buffer_;
    page_offset offset_;
};

}


#endif // __PAGANINI_ROW_BINARY_STREAM_H__
