#include "config.h"
#include <paganini/concurrency/pthread/Mutex.h>
#include <paganini/concurrency/SystemError.h>


namespace paganini
{
namespace concurrency
{
namespace pthread
{


Mutex::Mutex()
{
    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    
    int code;
    if ((code = pthread_mutex_init(&mutex_, &attrs)) < 0)
        throw SystemError("Cannot create the mutex", code);
        
    pthread_mutexattr_destroy(&attrs);
}


Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex_);
}


void Mutex::lock()
{
    int code;
    if ((code = pthread_mutex_lock(&mutex_)) < 0)
        throw SystemError("Cannot lock the mutex", code);
}


void Mutex::unlock()
{
    int code;
    if ((code = pthread_mutex_unlock(&mutex_)) < 0)
        throw SystemError("Cannot unlock the mutex", code);
}

} // pthread
} // paganini
} // concurrency

