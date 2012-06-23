/*
    Funkcje do wysylania danych przez sockety.
*/
#ifndef __PAGANINI_NETWORKING_SEND_H__
#define __PAGANINI_NETWORKING_SEND_H__

#include <paganini/networking/NetworkingError.h>
#include <paganini/networking/buffers.h>
#include <array>
#include <string>
using std::string;


namespace paganini
{
namespace networking
{

// Zapisuje do podanego obiektu udostepniajacego funkcje send() przyjmujaca
// Buffer calosc wiadomosci, poprzez potencjalnie kilka wywolan send().
template <typename _Sender, typename _ConstBufferLike>
std::size_t write(_Sender& sender, _ConstBufferLike buffer)
{
    ConstBuffer b = make_buffer(buffer);
    std::size_t total = 0;
    while (total < b.size)
    {
        total += sender.send(b.offset(total));
    }
    return total;
}


} // networking
} // paganini

#endif // __PAGANINI_NETWORKING_SEND_H__

