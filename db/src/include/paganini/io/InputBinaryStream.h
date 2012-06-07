/*
    Klasa pozwalajaca wczytywac dane z pamieci w przenosny sposob. Do ich
    zapisu sluzy klasa OutputBinaryStream.
*/
#ifndef __PAGANINI_ROW_INPUT_BINARY_STREAM_H__
#define __PAGANINI_ROW_INPUT_BINARY_STREAM_H__

#include <paganini/paging/types.h>
#include <paganini/io/BinaryStream.h>
#include <cstring>
#include <iterator>
#include <iostream>

namespace paganini
{


class InputBinaryStream: public BinaryStream
{
public:
    InputBinaryStream(raw_data buffer, size16 offset = 0);
    
    // Metody statyczne, przy ich pomocy sa implementowane zwykle wersje
    // Wczytuje z podanej lokacji obiekt typu T pod adres output.
    // Zwraca ilosc wczytanych bajtow.
    template <typename T>
    static size16 read(const_raw_data buffer, T* output);
    
    // Wczytuje z podanego bufora wiele (count) obiektow, umieszcza je 
    // w miejscu wskazywanym przez iterator out. Zwraca sumaryczna ilosc
    // wczytanych bajtow.
    template <typename OutputIter>
    static size16 readRange(const_raw_data buffer, OutputIter out, 
        size16 count);
    
    // Wczytuje z podanego bufora length bajtow, i umieszcza je w buforze
    // wyjsciowym. Zwraca ilosc przeczytanych bajtow (rowna length)
    static size16 readData(const_raw_data buffer, raw_data output, 
        size16 length);
    
    // Metody niestatyczne, realizuja swoje funkcje przy uzyciu powyzszych
    // statycznych
    
    // Wczytuje pod podany adres obiekt typu T. Zwraca ilosc przeczytanych 
    // bajtow.
    template <typename T>
    size16 read(T* output);
    
    // Wczytuje w miejsca wskazywane przez iterator out wiele (count) obiektow
    // typu determinowanego przez iterator. Zwraca sumaryczna ilosc 
    // przeczytanych bajtow.
    template <typename OutputIter>
    size16 readRange(OutputIter out, size16 count);
    
    // Wczytuje do podanego bufora wyjsciowego length bajtow. Zwraca ilosc
    //przeczytanych bajtow.
    size16 readData(raw_data output, size16 length);
};

// Specjalizacje dla typow zmiennoprzecinkowych, by zagwarantowac przenosnosc 

template <>
size16 InputBinaryStream::read<float>(const_raw_data buffer, float* output);

template<>
size16 InputBinaryStream::read<double>(const_raw_data buffer, double* output);


// Implementacje

template <typename T>
size16 InputBinaryStream::read(const_raw_data buffer, T* output)
{
    *output = T();
    for (int i = 0; i < sizeof(T); ++ i)
    {
        *output |= ((static_cast<T>(*buffer++) & 0xff) << (i * 8));
    }
    return sizeof(T);
}


template <typename OutputIter>
size16 InputBinaryStream::readRange(const_raw_data buffer, OutputIter out, 
    size16 count)
{
    size16 sum = 0;
    for (int i = 0; i < count; ++ i)
    {
        // Zwykly read przyjmuje wskaznik, wiec bierzemy adres z
        // dereferencji iteratora
        sum += read(buffer + sum, &(*(out ++)));
    }
    return sum;
}


template <typename T>
size16 InputBinaryStream::read(T* output)
{
    size16 taken = InputBinaryStream::read(getBuffer(), output);
    skip(taken);
    return taken;
} 


template <typename OutputIter>
size16 InputBinaryStream::readRange(OutputIter out, size16 count)
{
    size16 taken = InputBinaryStream::readRange(getBuffer(), out, count);
    skip(taken);
    return taken;
}


}


#endif // __PAGANINI_ROW_INPUT_BINARY_STREAM_H__
