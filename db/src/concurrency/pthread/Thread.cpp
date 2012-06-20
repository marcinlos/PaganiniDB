#include "config.h"
#include <paganini/concurrency/pthread/Thread.h>
#include <paganini/concurrency/SystemError.h>
#include <unistd.h>
#include <sys/syscall.h>


namespace paganini
{
namespace concurrency
{
namespace pthread
{


Thread::Thread(const Function& function): function_(function), id_(-1)
{
}


Thread::Thread(Thread&& other): thread_(other.thread_), 
    function_(std::move(other.function_)), id_(other.id_)
{
    other.id_ = -1;
}



Thread::Id Thread::self()
{
    return static_cast<Id>(syscall(SYS_gettid));
}


void Thread::operator ()()
{
    if (id_ == -1)
    {
        void* this_ = reinterpret_cast<void*>(this);
        int errcode = pthread_create(&thread_, nullptr, &run_thread_, this_);
        if (errcode != 0)
            throw SystemError("Failed to create a thread", errcode);
    }
    else
        throw std::logic_error("Trying to run thread more than once");
}


void Thread::join()
{
    if (id_ != -1)
    {
        int errcode = pthread_join(thread_, nullptr);
        if (errcode != 0)
            throw SystemError("Error while joining thread", errcode);
    }
    else
        throw std::logic_error("Trying to wait for thread which was not run");
}


void* Thread::run_thread_(void* self_pointer)
{
    Thread* this_ = reinterpret_cast<Thread*>(self_pointer);
    this_->id_ = self();
    this_->function_();
    return nullptr;
}



} // pthread
} // concurrency
} // paganini

