/*
    Funkcje pomocnicze do manipulacji na bitach. Uzywane glownie przez
    managera stronnicowania i system UV.
*/
#ifndef __PAGANINI_UTIL_BITS_H__
#define __PAGANINI_UTIL_BITS_H__

#include <paganini/paging/types.h>


namespace paganini
{

namespace util
{

// Zwraca najmniejsza ilosc bajtow zdolna pomiescic tyle bitow
inline int min_bytes(int bit_count)
{
    return (bit_count + 7) / 8;
}

// Ustawia w data bit o podanym numerze
inline void set_bit(raw_data data, int bit)
{
    data += bit / 8;
    *data |= (1 << (bit % 8));
}


// Zeruje w data bit o podanym numerze
inline void unset_bit(raw_data data, int bit)
{
    data += bit / 8;
    *data &= ~(1 << (bit % 8));
}


// Zwraca wartosc bitu o podanym numerze
inline bool get_bit(const_raw_data data, int bit)
{
    data += bit / 8;
    return ((*data) & (1 << (bit % 8))) != 0;
}


// Znajduje niezerowy bit w podanym bajcie. Jesli go nie ma, zwraca -1.
// Perfidne wyszukiwanie binarne... 
inline int first_nonzero_bit(unsigned char byte)
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


} // util
} // paganini

#endif // __PAGANINI_UTIL_BITS_H__


