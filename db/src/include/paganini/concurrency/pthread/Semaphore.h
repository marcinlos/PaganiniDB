/*
    Klasa opakowujaca semafor pthreadowy.
*/
#ifndef __PAGANINI_CONCURRENCY_PTHREAD_SEMAPHORE_H__
#define __PAGANINI_CONCURRENCY_PTHREAD_SEMAPHORE_H__

#include <semaphore.h>
#include <memory>


namespace paganini
{
namespace concurrency
{
namespace pthread
{


class Semaphore
{
public:
    Semaphore(int init);
    
    Semaphore(const Semaphore&) = delete;
    
    ~Semaphore();
    
    // Opuszcza semafor
    void lock();
    
    // Podnosi semafor
    void unlock();
    
private:
    sem_t semaphore_;
};


} // pthread
} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_PTHREAD_SEMAPHORE_H__

