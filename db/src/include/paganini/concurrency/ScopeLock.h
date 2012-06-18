/*
    Prosty szablon pozwalajacy blokowac zasoby udostepniajace funkcje
    lock i unlock z uzyciem techniki RAII.
*/
#ifndef __PAGANINI_CONCURRENCY_SCOPE_LOCK_H__
#define __PAGANINI_CONCURRENCY_SCOPE_LOCK_H__


namespace paganini
{
namespace concurrency
{


template <class _Lockable>
class ScopeLock
{
public:
    // Zapamietuje i lockuje podany obiekt 
    explicit ScopeLock(_Lockable& lockable);
    
    // Przenosi locka bez lockowania/unlockowania
    ScopeLock(ScopeLock&& other);
    
    // Zwalnia locka
    ~ScopeLock();
    
private:
    _Lockable* lockable_;
};


template <class _Lockable>
ScopeLock<_Lockable> make_lock(_Lockable& lockable)
{
    return ScopeLock<_Lockable>(lockable);
}


template <class _Lockable>
ScopeLock<_Lockable>::ScopeLock(_Lockable& lockable): lockable_(&lockable)
{
    lockable_->lock();
}


template <class _Lockable>
ScopeLock<_Lockable>::ScopeLock(ScopeLock&& other): 
    lockable_(other.lockable_)
{
    other.lockable_ = nullptr;
}


template <class _Lockable>
ScopeLock<_Lockable>::~ScopeLock()
{
    if (lockable_ != nullptr)
        lockable_->unlock();
}


} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_SCOPE_LOCK_H__

