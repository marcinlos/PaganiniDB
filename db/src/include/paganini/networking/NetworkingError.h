/*
    Klasa bledu komunikacji sieciowej.
*/
#ifndef __PAGANINI_NETWORKING_NETWORKING_ERROR_H__
#define __PAGANINI_NETWORKING_NETWORKING_ERROR_H__

#include <stdexcept>


namespace paganini
{
namespace networking
{


class NetworkingError: virtual public std::runtime_error
{
public:
    NetworkingError(const string& description, int code): 
        std::runtime_error(description), error_code_(code)
    {
    }
    
    int errorCode() const
    {
        return error_code_;
    }
    
protected:
    int error_code_;
};


} // networking
} // paganini


#endif // __PAGANINI_NETWORKING_NETWORKING_ERROR_H__

