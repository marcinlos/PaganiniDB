#include "config.h"
#include <paganini/concurrency/MutexPthread.h>
#include <paganini/concurrency/SystemError.h>


namespace paganini
{
namespace concurrency
{


MutexPthread::MutexPthread(): mutex_(new pthread_mutex_t)
{
    int code;
    if ((code = pthread_mutex_init(mutex_.get(), nullptr)) < 0)
        throw SystemError("Cannot create the mutex", code);
}


MutexPthread::~MutexPthread()
{
    pthread_mutex_destroy(mutex_.get());
}


void MutexPthread::lock()
{
    int code;
    if ((code = pthread_mutex_lock(mutex_.get())) < 0)
        throw SystemError("Cannot lock the mutex", code);
}


void MutexPthread::unlock()
{
    int code;
    if ((code = pthread_mutex_unlock(mutex_.get())) < 0)
        throw SystemError("Cannot unlock the mutex", code);
}


} // paganini
} // concurrency
