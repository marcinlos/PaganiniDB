/*
    Funkcje pomocnicze do manipulacji na bitach. Uzywane glownie przez
    managera stronnicowania i system UV.
*/
#ifndef __PAGANINI_UTIL_BITS_H__
#define __PAGANINI_UTIL_BITS_H__

#include <cstdint>

namespace paganini
{

namespace util
{

// Ustawia w data bit o podanym numerze
inline void set_bit(uint8_t* data, int bit)
{
    data += bit / 8;
    *data |= (1 << (bit % 8));
}


// Zeruje w data bit o podanym numerze
inline void unset_bit(uint8_t* data, int bit)
{
    data += bit / 8;
    *data &= ~(1 << (bit % 8));
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


