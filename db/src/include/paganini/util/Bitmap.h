/*
    Klasa reprezentujaca dynamicznie ustalanej dlugosci ciag bitow.
    Udostepnia iteratory oraz proxy pozwalajace manipulowac pojedynczymi
    bitami.
*/
#ifndef __PAGANINI_UTIL_BITMAP_H__
#define __PAGANINI_UTIL_BITMAP_H__

#include <paganini/paging/types.h>
#include <paganini/util/bits.h>
#include <vector>

namespace paganini
{

namespace util
{


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


#endif // #ifndef __PAGANINI_UTIL_BITMAP_H__

