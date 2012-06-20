/*
    Klasa opakowujaca zwykly mutex pthreadowy.
*/
#ifndef __PAGANINI_CONCURRENCY_PTHREAD_MUTEX_H__
#define __PAGANINI_CONCURRENCY_PTHREAD_MUTEX_H__

#include <pthread.h>


namespace paganini
{
namespace concurrency
{
namespace pthread
{


class Mutex
{
public:
    // Tworzy mutex z domyslnymi atrybutami
    Mutex();

    // Niszczy zasoby mutexu
    ~Mutex();
    
    Mutex(const Mutex&) = delete;
    
    // Blokuje mutex
    void lock();
    
    // Zwalnia mutex
    void unlock();
    
private:
    pthread_mutex_t mutex_;
};


} // pthread
} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_PTHREAD_MUTEX_H__

