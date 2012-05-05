/*
*/
#ifndef __PAGANINI_UTIL_SINGLETON_H__
#define __PAGANINI_UTIL_SINGLETON_H__

namespace paganini
{
namespace util
{


template <typename T>
class Singleton
{
protected:
    Singleton() { } ;
    Singleton(const Singleton&) = delete;
    Singleton& operator = (const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    
public:
    static T& getInstance()
    {
        static T object;
        return object;
    }
};


} // util
} // paganini


#endif // __PAGANINI_UTIL_SINGLETON_H__
