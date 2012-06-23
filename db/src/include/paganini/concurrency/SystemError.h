/*
    Klasa wyjatku biblioteki pthread.
*/
#ifndef __PAGANINI_CONCURRENCY_SYSTEM_ERROR_H__
#define __PAGANINI_CONCURRENCY_SYSTEM_ERROR_H__

#include <stdexcept>
#include <string>
using std::string;


namespace paganini
{
namespace concurrency
{


class SystemError: virtual public std::runtime_error
{
public:
    SystemError(const string& description, int code): 
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


} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_SYSTEM_ERROR_H__
