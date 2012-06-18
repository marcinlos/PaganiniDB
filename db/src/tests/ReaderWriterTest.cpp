#include "config.h"
#include <paganini/concurrency/MutexPthread.h>
#include <paganini/concurrency/SemaphorePthread.h>
#include <paganini/concurrency/ReadWriteLock.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>
#include <vector>

typedef paganini::concurrency::MutexPthread Mutex;
typedef paganini::concurrency::SemaphorePthread Semaphore;
typedef paganini::concurrency::ReadWriteLock<Mutex, Semaphore> ReadWriteLock;

ReadWriteLock rw_lock;

inline void sleep_ms(int low, int high)
{
    usleep((low + (rand() % (high - low))) * 1000);
}


void* reader_thread(void*)
{
    pid_t self = static_cast<pid_t>(syscall(SYS_gettid));
    while (true)
    {
        rw_lock.lockRead();
        std::cout << "[" << self << "] Czytam" << std::endl;
        sleep_ms(400, 1500);
        rw_lock.unlockRead();
        sleep_ms(1000, 2000);
    }
    return nullptr;
}


void* writer_thread(void*)
{
    pid_t self = static_cast<pid_t>(syscall(SYS_gettid));
    while (true)
    {
        rw_lock.lockWrite();
        std::cout << "[" << self << "] Pisze" << std::endl;
        sleep_ms(400, 1500);
        rw_lock.unlockWrite();
        sleep_ms(1700, 2900);
    }
    return nullptr;
}


int main()
{
    static const int THREAD_COUNT = 5;
    pthread_t threads[THREAD_COUNT * 2];
    
    for (int i = 0; i < THREAD_COUNT; ++ i)
        pthread_create(&threads[i], NULL, &reader_thread, NULL);
    for (int i = 0; i < THREAD_COUNT; ++ i)
        pthread_create(&threads[THREAD_COUNT + i], NULL, &writer_thread, NULL);
        
    for (int i = 0; i < THREAD_COUNT * 2; ++ i)
        pthread_join(threads[i], NULL);
    return 0;
}

