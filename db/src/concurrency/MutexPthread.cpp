#include "config.h"
#include <paganini/concurrency/MutexPthread.h>
#include <paganini/concurrency/SystemError.h>


namespace paganini
{
namespace concurrency
{


MutexPthread::MutexPthread()
{
    int code;
    if ((code = pthread_mutex_init(&mutex_, nullptr)) < 0)
        throw SystemError("Cannot create the mutex", code);
}


MutexPthread::~MutexPthread()
{
    pthread_mutex_destroy(&mutex_);
}


void MutexPthread::lock()
{
    int code;
    if ((code = pthread_mutex_lock(&mutex_)) < 0)
        throw SystemError("Cannot lock the mutex", code);
}


void MutexPthread::unlock()
{
    int code;
    if ((code = pthread_mutex_unlock(&mutex_)) < 0)
        throw SystemError("Cannot unlock the mutex", code);
}


} // paganini
} // concurrency
