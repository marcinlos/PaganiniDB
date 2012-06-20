#include "config.h"
#include <paganini/concurrency/pthread/Semaphore.h>
#include <paganini/concurrency/SystemError.h>
#include <errno.h>


namespace paganini
{
namespace concurrency
{
namespace pthread
{


Semaphore::Semaphore(int init)
{
    if (sem_init(&semaphore_, 0, init) < 0)
        throw SystemError("Cannot create the mutex", errno);
}


Semaphore::~Semaphore()
{
    sem_destroy(&semaphore_);
}


void Semaphore::lock()
{
    if (sem_wait(&semaphore_) < 0)
        throw SystemError("Cannot lock the mutex", errno);
}


void Semaphore::unlock()
{
    if (sem_post(&semaphore_) < 0)
        throw SystemError("Cannot unlock the mutex", errno);
}


} // pthread
} // concurrency
} // paganini
