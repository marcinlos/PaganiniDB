/*
    Klasa reprezentujaca socket z uzyciem api systemu UNIX.
*/
#ifndef __PAGANINI_HEADER_NETWORKING_UNIX_SOCKET_H__
#define __PAGANINI_HEADER_NETWORKING_UNIX_SOCKET_H__

#include <paganini/networking/NetworkingError.h>
#include <paganini/networking/buffers.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <utility>


namespace paganini
{
namespace networking
{
namespace unix
{

template <typename _Address>
class Socket 
{
public:
    typedef int NativeType;
    typedef typename _Address::NativeType NativeAddress;
    typedef sockaddr BaseAddress;
    
    static const int MAX_CONNECTIONS = 6;
    
    // Tworzy nowy socket
    Socket();
    
    // Tworzy socket z podanego deskryptora
    explicit Socket(NativeType socket);
    
    // Przenosi socket
    Socket(Socket&& other);
    
    // Non-copyable
    Socket(const Socket&) = delete;
    
    // Zamyka socket
    ~Socket();
    
    // Zamyka socket
    void close();
    
    void bind(const _Address& address);
    
    void connect(const _Address& address);
    
    void listen(int backlog = MAX_CONNECTIONS) const;
    
    // Funkcja do wysylania danych. Zwraca ilosc rzeczywiscie wyslanych
    // bajtow. Nie gwarantuje otrzymania calosci danych.
    std::size_t send(ConstBuffer buffer);
    
    // Funkcja do odbioru danych. Zwraca ilosc przeczytanych danych.
    // Nie gwarantuje zapelnienia bufora.
    std::size_t receive(Buffer buffer);
    
    // Zwraca deskryptor
    NativeType native() const;

private:
    // Deskryptor socketu
    int socket_desc_;
};


template <typename _Address>
Socket<_Address>::Socket()
{
    if ((socket_desc_ = socket(_Address::CREATE_FLAG, SOCK_STREAM, 0)) < 0)
    {
        throw NetworkingError("Failed to create socket", errno);
    }        
}


template <typename _Address>
Socket<_Address>::Socket(Socket&& other): socket_desc_(other.socket_desc_)
{
    other.socket_desc_ = -1;
}


template <typename _Address>
Socket<_Address>::Socket(NativeType socket): socket_desc_(socket)
{
}


template <typename _Address>
Socket<_Address>::~Socket()
{
    close();
}


template <typename _Address>
void Socket<_Address>::close()
{
    if (socket_desc_ > 0)
    {
        if (::close(socket_desc_) < 0)
        {
            throw NetworkingError("Failed to close socket", errno);
        }    
        socket_desc_ = -1;    
    }
    else 
        throw std::logic_error("Trying to close socket which was not opened");
}


template <typename _Address>
void Socket<_Address>::bind(const _Address& address)
{
    NativeAddress addr = address.toNative();
    int err_code = ::bind(socket_desc_, reinterpret_cast<BaseAddress*>(&addr),
        sizeof(addr));
    if (err_code != 0)
    {
        throw NetworkingError(util::format("Failed to bind socket to "
            "address {}", address), errno);
    }
}


template <typename _Address>
void Socket<_Address>::connect(const _Address& address)
{
    NativeAddress addr = address.toNative();
    int err_code = ::connect(socket_desc_, reinterpret_cast<BaseAddress*>(&addr),
        sizeof(addr));
    if (err_code != 0)
    {
        throw NetworkingError(util::format("Failed to connect socket to "
            "address {}", address), errno);
    }
}


template <typename _Address>
void Socket<_Address>::listen(int backlog) const
{
    if (::listen(socket_desc_, backlog) < 0)
    {
        throw NetworkingError("Failed to listen()", errno);
    }
}


template <typename _Address>
typename Socket<_Address>::NativeType Socket<_Address>::native() const
{
    return socket_desc_;
}


template <typename _Address>
std::size_t Socket<_Address>::send(ConstBuffer buffer)
{
    int ret = ::write(socket_desc_, buffer.data, buffer.size);
    if (ret < 0)
	{
	    throw NetworkingError("Failed to write to socket", errno);
    }
    return static_cast<std::size_t>(ret);
}


template <typename _Address>
std::size_t Socket<_Address>::receive(Buffer buffer)
{
    int ret = ::read(socket_desc_, buffer.data, buffer.size);
    if (ret < 0)
	{
	    throw NetworkingError("Failed to write to socket", errno);
    }
    return static_cast<std::size_t>(ret);
}


} // unix
} // networking
} // paganini


#endif // __PAGANINI_HEADER_NETWORKING_UNIX_SOCKET_H__

