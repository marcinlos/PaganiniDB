#include "config.h"
#include <paganini/concurrency/ThreadPageLocker.h>
#include <paganini/concurrency/pthread/Thread.h>
#include <paganini/concurrency/pthread/Mutex.h>
#include <paganini/concurrency/pthread/Semaphore.h>
#include <paganini/concurrency/ReadWriteLock.h>
#include <paganini/concurrency/ScopeLock.h>
#include <paganini/util/format.h>
#include <paganini/concurrency/SystemError.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <string.h>
using std::string;
using paganini::util::format;
using std::cout;
using std::cerr;
using std::endl;
using paganini::concurrency::pthread::Mutex;
using paganini::concurrency::pthread::Semaphore;
using paganini::concurrency::pthread::Thread;
using paganini::concurrency::ReadWriteLock;
using paganini::concurrency::make_lock;

using paganini::page_number;

typedef paganini::concurrency::ThreadPageLocker<Thread, Mutex, ReadWriteLock<Mutex, Semaphore>> PageLocker;

typedef PageLocker::ReadLock ReadLock;
typedef PageLocker::WriteLock WriteLock;


void process_input(PageLocker& locker)
{
    string command;
    int page;
    while (std::cin)
    {
        std::cin >> command >> page;
        if (! std::cin)
        {
            std::cout << "Invalid page number" << std::endl;
            std::cin.sync();
            continue;
        }
        if (command == "r")
            locker.pageLockRead(page);
        else if (command == "ur")
            locker.pageUnlockRead(page);
        else if (command == "w")
            locker.pageLockWrite(page);
        else if (command == "uw")
            locker.pageUnlockWrite(page);
        std::cout << "Done!" << std::endl;
    }
}

Mutex state;
int r;
bool w;
inline void check()
{
    if (r > 0 && w)
        std::cerr << "Damn :(" << std::endl;
}

static const int PAGES = 3;

void run_reader(PageLocker& locker)
{
    Thread::Id pid = Thread::self();
    int i = 0;
    while (true)
    {
        ++ i;
        page_number page = rand() % PAGES;
        cerr << format("[{}] ({}) Want to read {}...", pid, i, page) << endl;
        ReadLock rl = locker.readLock(page);
        auto slock = make_lock(state);
        ++ r;
        check();
        //usleep(10000);
        -- r;
        cerr << format("[{}] ({}) Reading from {}...", pid, i, page) << endl;
    }
}


void run_writer(PageLocker& locker)
{
    Thread::Id pid = Thread::self();
    int i = 0;
    while (true)
    {
        ++ i;
        page_number page = rand() % PAGES;
        cerr << format("[{}] ({}) Want to write {}...", pid, i, page) << endl;
        WriteLock wl = locker.writeLock(page);
        auto slock = make_lock(state);
        w = true;
        check();
        //usleep(10000);
        w = false;
        cerr << format("[{}] ({}) Writing to {}...", pid, i, page) << endl; 
    }
}


int main(int argc, char* argv[])
{
    static const int READERS = 50, WRITERS = 10;
    try
    {
        std::cerr << Thread::self() << endl;
        PageLocker locker;  
        std::function<void ()> f(std::bind<void>(run_reader, std::ref(locker)));
        //f();
        std::vector<Thread> threads;
        /*for (int i = 0; i < READERS; ++ i)
        {
            threads.emplace_back(run_reader, std::ref(locker));
        }*/
        for (int i = 0; i < WRITERS; ++ i)
        {
            threads.emplace_back(run_writer, std::ref(locker));
        }
        for (Thread& thread: threads)
            thread();
        sleep(1);
        for (Thread& thread: threads)
            thread.join();
    }
    catch (paganini::concurrency::SystemError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "Error: " << strerror(e.errorCode()) << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

