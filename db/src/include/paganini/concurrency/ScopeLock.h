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
    
    // Odblokowuje locka
    void unlock();
    
    // Zwalnia locka
    ~ScopeLock();
    
private:
    _Lockable& lockable_;
    bool locked_;
};


template <class _Lockable>
ScopeLock<_Lockable> make_lock(_Lockable& lockable)
{
    return ScopeLock<_Lockable>(lockable);
}


template <class _Lockable>
ScopeLock<_Lockable>::ScopeLock(_Lockable& lockable): lockable_(lockable)
{
    lockable_.lock();
    locked_ = true;
}


template <class _Lockable>
ScopeLock<_Lockable>::ScopeLock(ScopeLock&& other): 
    lockable_(other.lockable_)
{
    other.locked_ = false;
}


template <class _Lockable>
void ScopeLock<_Lockable>::unlock()
{
    if (locked_)
    {
        lockable_.unlock();
        locked_ = false;
    }
}


template <class _Lockable>
ScopeLock<_Lockable>::~ScopeLock()
{
    unlock();
}


} // concurrency
} // paganini


#endif // __PAGANINI_CONCURRENCY_SCOPE_LOCK_H__

