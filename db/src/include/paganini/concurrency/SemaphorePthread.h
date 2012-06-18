/*
    Klasa opakowujaca semafor pthreadowy.
*/
#ifndef __PAGANINI_CONCURRENCY_SEMAPHORE_PTHREAD_H__
#define __PAGANINI_CONCURRENCY_SEMAPHORE_PTHREAD_H__

#include <semaphore.h>
#include <memory>


namespace paganini
{
namespace concurrency
{


class SemaphorePthread
{
public:
    SemaphorePthread(int init);
    
    SemaphorePthread(const SemaphorePthread&) = delete;
    
    ~SemaphorePthread();
    
    // Opuszcza semafor
    void lock();
    
    // Podnosi semafor
    void unlock();
    
private:
    sem_t semaphore_;
};


} // concurrency
} // paganini



#endif // __PAGANINI_CONCURRENCY_SEMAPHORE_PTHREAD_H__
