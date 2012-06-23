/*
    Klasa sluzaca do akceptowania polaczen.
*/
#ifndef __PAGANINI_NETWORKING_UNIX_ACCEPTOR_H__
#define __PAGANINI_NETWORKING_UNIX_ACCEPTOR_H__

#include <paganini/paging/types.h>
#include <paganini/networking/unix/Socket.h>


namespace paganini
{
namespace networking
{
namespace unix
{


template <typename _Address>
class Acceptor
{
public:
    Acceptor(const _Address& address);
    
    // Czeka na przychodzace polaczenie
    Socket<_Address> accept();

private:
    typedef typename Socket<_Address>::NativeType NativeSocket;
    typedef typename _Address::NativeType NativeAddress;
    typedef typename Socket<_Address>::BaseAddress BaseAddress;
    Socket<_Address> socket_;
};


template <typename _Address>
Acceptor<_Address>::Acceptor(const _Address& address)
{
    socket_.bind(address);
    socket_.listen();
}


template <typename _Address>
Socket<_Address> Acceptor<_Address>::accept()
{
    NativeSocket desc = socket_.native();
    NativeAddress addr;
    unsigned int size;
    NativeSocket result = ::accept(desc, reinterpret_cast<BaseAddress*>(&addr),
        &size);
    if (result < 0)
    {
        throw NetworkingError("Failed to accept connection", errno);
    }
    return Socket<_Address>(result);
}


} // unix
} // networking
} // paganini

#endif // __PAGANINI_NETWORKING_UNIX_ACCEPTOR_H__

