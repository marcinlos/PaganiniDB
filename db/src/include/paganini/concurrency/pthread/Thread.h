/*
    Klasa reprezentujaca watek pthreadowy, 
*/
#ifndef __PAGANINI_CONCURRENCY_PTHREAD_THREAD_H__
#define __PAGANINI_CONCURRENCY_PTHREAD_THREAD_H__

#include <paganini/concurrency/pthread/Mutex.h>
#include <paganini/concurrency/pthread/Semaphore.h>
#include <paganini/concurrency/ReadWriteLock.h>
#include <pthread.h>
#include <functional>


namespace paganini
{
namespace concurrency
{
namespace pthread
{


class Thread
{
public:
    typedef pid_t Id;
    typedef Mutex DefaultMutex;
    typedef ReadWriteLock<Mutex, Semaphore> DefaultReadWriteLock;
    typedef std::function<void ()> Function;
    static Id self();
    
    // Non-copyable
    Thread(const Thread&) = delete;
    
    // Movable
    Thread(Thread&& other);
    
    // Tworzy watek z podanej funkcji
    Thread(const Function& function);
    
    // Tworzy watek z podanej funkcji i jej argumentow
    template <typename F, typename... Args>
    Thread(F&& function, Args&&... args);
    
    void operator ()();
    void join();
    
    
private:
    pthread_t thread_;
    std::function<void ()> function_;
    Id id_;
    
    static void* run_thread_(void* self_pointer); 
};


template <typename F, typename... Args>
Thread::Thread(F&& function, Args&&... args):
    function_(std::bind<void>(std::forward<F>(function), 
        std::forward<Args>(args)...)), id_(-1)
{
}


} // pthread
} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_PTHREAD_THREAD_H__

