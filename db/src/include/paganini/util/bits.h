/*
    Funkcje pomocnicze do manipulacji na bitach. Uzywane glownie przez
    managera stronnicowania i system UV.
*/
#ifndef __PAGANINI_UTIL_BITS_H__
#define __PAGANINI_UTIL_BITS_H__

#include <paganini/paging/types.h>
#include <cstdint>
#include <vector>

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


// Dynamiczny bitset, brakuje go niestety w stl-u.
class Bitmap
{
private:
    std::vector<char> _data;
    size16 _bits;
public:
    typedef std::vector<char>::iterator bytes_iterator;
    typedef std::vector<char>::const_iterator const_bytes_iterator;
    

    Bitmap(size16 bits): _data(min_bytes(bits)), _bits(bits)
    {
    }
    
    // Wewnetrzny typ symulujacy zachowanie "referencji na bit"
    class BitProxy
    {
    private:
        Bitmap& bitmap;
        const int index;
        
    public:
        BitProxy(Bitmap& bitmap, int index): bitmap(bitmap), index(index)
        {
        }
        
        // Zwraca wartosc boolowska z oryginalnej tablicy
        operator bool () const 
        { 
            return get_bit(&bitmap._data[0], index); 
        }
        
        // Przypisanie - razem z powyzszym pozwalaja w wiekszosci przypadkow
        // traktowac klase jako 'referencje na bit'
        BitProxy& operator = (bool val)
        {
            val ? set_bit(&bitmap._data[0], index) : 
                unset_bit(&bitmap._data[0], index);
            return *this;
        }
    };
    
    // Dostep poprzez indeksy
    BitProxy operator [] (int i) { return { *this, i }; }
    bool operator [] (int i) const { return get_bit(&_data[0], i); }
    
    // Zwraca pamiec, w ktorej przechowywane sa bity
    const std::vector<char>& raw_data() const { return _data; }
    std::vector<char>& raw_data() { return _data; }
    
    // Zwraca ilosc przechowywanych bitow
    size16 size() const { return _bits; }
    
    // Iteracja po bajtach. Glownie ze wzgledu na koniecznosc zapisywania
    // binarnego.
    bytes_iterator bytes_begin() { return _data.begin(); }
    bytes_iterator bytes_end() { return _data.end(); }
    
    const_bytes_iterator bytes_begin() const { return _data.begin(); }
    const_bytes_iterator bytes_end() const { return _data.end(); }
};



} // util
} // paganini

#endif // __PAGANINI_UTIL_BITS_H__


