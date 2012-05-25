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
protected:
    raw_data buffer;
    page_offset offset;
    
public:
    BinaryStream(raw_data buffer, size16 offset = 0);
       
    // Pozostale metody
    page_offset getOffset() const;
    
    void setOffset(page_offset offset);
    
    void skip(page_offset offset);
    
    raw_data getBuffer();
};

}


#endif // __PAGANINI_ROW_BINARY_STREAM_H__
