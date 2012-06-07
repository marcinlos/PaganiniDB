/*
    Klasa pozwalajaca zapisywac dane do pamieci w przenosny sposob. Do ich
    odczytywania slozy inna klasa, InputBinaryStream.
*/
#ifndef __PAGANINI_ROW_OUTPUT_BINARY_STREAM_H__
#define __PAGANINI_ROW_OUTPUT_BINARY_STREAM_H__

#include <paganini/paging/types.h>
#include <paganini/row/BinaryStream.h>
#include <cstring>

namespace paganini
{


class OutputBinaryStream: public BinaryStream
{   
public:
    OutputBinaryStream(raw_data buffer, size16 offset = 0);
    
    // Metody statyczne, przy ich pomocy sa implementowane zwykle wersje
    
    // Zapisuje podana wartosc do bufora. Zwraca ilosc zapisanych bajtow.
    template <typename T>
    static size16 write(raw_data buffer, const T& value);
    
    // Zapisuje podany zakres wartosci do podanego bufora. Zwraca sumaryczna
    // ilosc bajtow.
    template <typename Iter>
    static size16 writeRange(raw_data buffer, Iter begin, Iter end);
    
    // Zapisuje do bufora length bajtow z podanej lokacji. Zwraca ilosc
    // zapisanych bajtow (length).
    static size16 writeData(raw_data buffer, const_raw_data data, 
        size16 length);
    
    // Metody niestatyczne, realizuja swoje funkcje przy uzyciu powyzszych
    // statycznych.
    
    // Zapisuje podana wartosc do bufora. Zwraca ilosc zapisanych bajtow.
    template <typename T>
    size16 write(T value);
    
    // Zapisuje podany ciag wartosci do bufora. Zwraca sumaryczna ilosc
    // zapisanych bajtow.
    template <typename Iter>
    size16 writeRange(Iter begin, Iter end);
    
    // Zapisuje length bajtow do bufora. Zwraca ilosc zapisanych bajtow.
    size16 writeData(const_raw_data data, size16 length);
};

// Specjalizacje dla typow zmiennoprzecinkowych, by zagwarantowac przenosnosc

template <>
size16 OutputBinaryStream::write<float>(raw_data buffer, const float& value);

template <>
size16 OutputBinaryStream::write<double>(raw_data buffer, const double& value);


// Implementacje

template <typename T>
size16 OutputBinaryStream::write(raw_data buffer, const T& value)
{
    for (int i = 0; i < sizeof(value); ++ i)
    {
        *buffer++ = ((value >> (8 * i)) & 0xff);
    }
    return sizeof(value);
}


template <typename Iter>
size16 OutputBinaryStream::writeRange(raw_data buffer, Iter begin, Iter end)
{
    size16 sum = 0;
    while (begin != end)
        sum += write(buffer + sum, *begin ++);
    return sum;
}


template <typename T>
size16 OutputBinaryStream::write(T value)
{
    size16 written = OutputBinaryStream::write(getBuffer(), value);
    skip(written);
    return written;
}


template <typename Iter>
size16 OutputBinaryStream::writeRange(Iter begin, Iter end)
{
    size16 written = OutputBinaryStream::writeRange(getBuffer(), begin, end);
    skip(written);
    return written;
}


}


#endif // __PAGANINI_ROW_OUTPUT_BINARY_STREAM_H__
