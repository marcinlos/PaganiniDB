#include "config.h"
#include <paganini/concurrency/FilePageLocker.h>
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
using std::endl;

typedef paganini::concurrency::FilePageLocker PageLocker;

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


void run_upgrader(PageLocker& locker)
{
    pid_t pid = getpid();
    int i = 0;
    while (true)
    {
        ++ i;
        {
            ReadLock rl = locker.readLock(0);
            cout << format("[{}] ({}) Reading...", pid, i) << endl;
            //sleep(3);
            {
                WriteLock wl = locker.writeLock(0);
                cout << format("[{}] ({}) Writing...", pid, i) << endl; 
                //sleep(5);
                cout << format("[{}] ({}) Done writing", pid, i) << endl;  
            }
            //sleep(2);
            cout <<  format("[{}] ({}) Done reading", pid, i) << endl;
        }
        //sleep(1);
    }
}


void run_reader(PageLocker& locker)
{
    pid_t pid = getpid();
    int i = 0;
    while (true)
    {
        ++ i;
        ReadLock rl = locker.readLock(0);
        cout << format("[{}] ({}) Reading...", pid, i) << endl;
        //sleep(2);
        std::cout << format("[{}] ({}) Done reading", pid, i)  << endl;
    }
}


void run_writer(PageLocker& locker)
{
    pid_t pid = getpid();
    int i = 0;
    while (true)
    {
        ++ i;
        WriteLock wl = locker.writeLock(0);
        cout << format("[{}] ({}) Writing...", pid, i) << endl; 
        //sleep(5);
        cout << format("[{}] ({}) Done writing", pid, i) << endl;
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: locks <filename> [mode]" << std::endl;
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
        std::cerr << "Error while opening file" << std::endl;
        std::cerr << strerror(errno) << std::endl;
    }
    try
    {
        PageLocker locker(fd);  
        if (argc >= 3)
        {
            string mode = argv[2];
            if (mode == "reader")
                run_reader(locker);
            else if (mode == "writer")
                run_writer(locker);
            else if (mode == "upg")
                run_upgrader(locker);
        }
        else
            process_input(locker);
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

