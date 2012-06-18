#include "config.h"
#include <paganini/concurrency/SemaphorePthread.h>
#include <paganini/concurrency/SystemError.h>
#include <errno.h>


namespace paganini
{
namespace concurrency
{


SemaphorePthread::SemaphorePthread(int init)
{
    if (sem_init(&semaphore_, 0, init) < 0)
        throw SystemError("Cannot create the mutex", errno);
}


SemaphorePthread::~SemaphorePthread()
{
    sem_destroy(&semaphore_);
}


void SemaphorePthread::lock()
{
    if (sem_wait(&semaphore_) < 0)
        throw SystemError("Cannot lock the mutex", errno);
}


void SemaphorePthread::unlock()
{
    if (sem_post(&semaphore_) < 0)
        throw SystemError("Cannot unlock the mutex", errno);
}


} // concurrency
} // paganini
