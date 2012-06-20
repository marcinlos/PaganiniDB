#include "config.h"
#include <paganini/concurrency/pthread/Mutex.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>
#include <vector>

using paganini::concurrency::pthread::Mutex;

Mutex mutex;


void* example_thread(void*)
{
    pid_t self = static_cast<pid_t>(syscall(SYS_gettid));
    while (true)
    {
        mutex.lock();
        std::cout << "[" << self << "] Zyje sobie" << std::endl;
        sleep(1);
        mutex.unlock();
        sleep(1);
    }
    return nullptr;
}


int main()
{
    static const int THREAD_COUNT = 5;
    pthread_t threads[THREAD_COUNT];
    
    for (int i = 0; i < THREAD_COUNT; ++ i)
        pthread_create(&threads[i], NULL, &example_thread, NULL);
        
    for (int i = 0; i < THREAD_COUNT; ++ i)
        pthread_join(threads[i], NULL);
    return 0;
}

