/*
    Klasa opakowujaca zwykly mutex pthreadowy.
*/
#ifndef __PAGANINI_CONCURRENCY_MUTEX_PTHREAD_H__
#define __PAGANINI_CONCURRENCY_MUTEX_PTHREAD_H__

#include <pthread.h>
#include <memory>


namespace paganini
{
namespace concurrency
{


class MutexPthread
{
public:
    // Tworzy mutex z domyslnymi atrybutami
    MutexPthread();
    
    // Niszczy zasoby mutexu
    ~MutexPthread();
    
    // Blokuje mutex
    void lock();
    
    // Zwalnia mutex
    void unlock();
    
private:
    std::unique_ptr<pthread_mutex_t> mutex_;
};


} // concurrency
} // paganini



#endif // __PAGANINI_CONCURRENCY_MUTEX_PTHREAD_H__