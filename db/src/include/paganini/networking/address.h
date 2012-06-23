/*
    Struktury reprezentujace adresy lokalne/sieciowe dla socketow.
*/
#ifndef __PAGANINI_NETWORKING_ADDRESS_H__
#define __PAGANINI_NETWORKING_ADDRESS_H__

#include <paganini/util/format.h>
#include <paganini/networking/NetworkingError.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <utility>
using std::string;


namespace paganini
{
namespace networking
{
namespace address
{

typedef unsigned short int port_number;


class Internet
{
public:
    string address;
    port_number port;
    
    typedef sockaddr_in NativeType;
    
    static const int CREATE_FLAG = AF_INET;
    
    // Tworzymy adres ze stringa i numeru portu
    Internet(string address = "", port_number port = 0);        
    
    Internet(const Internet& other);
    Internet(Internet&& other);

    NativeType toNative() const;
};


Internet::Internet(string address, port_number port):
    address(std::move(address)), port(port)
{
}


Internet::Internet(const Internet& other): address(other.address),
    port(other.port)
{
}


Internet::Internet(Internet&& other): address(std::move(other.address)),
    port(other.port)
{
}


Internet::NativeType Internet::toNative() const
{
    NativeType addr;
    //addr.sin_len = sizeof(addr);
    addr.sin_family =  AF_INET;
    if (address.empty())
    {
        if (::inet_aton(address.c_str(), &addr.sin_addr) != 0)
        {
            throw NetworkingError(util::format("'{}' is not a valid "
                "address", address), errno);
        }
    }
    else
        addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    return addr;
}


std::ostream& operator << (std::ostream& os, const Internet& inet)
{
    return os << (inet.address.empty() ? "(?)" : inet.address) 
        << ":" << inet.port; 
}


} // address
} // networking
} // paganini


#endif // __PAGANINI_NETWORKING_ADDRESS_H__

