#ifndef __PAGANINI_PAGING_DUMMY_LOCKER_H__
#define __PAGANINI_PAGING_DUMMY_LOCKER_H__

#include <paganini/paging/types.h>


namespace paganini
{


class DummyLocker
{
public:
    typedef int ReadLock;
    typedef int WriteLock;
    
    inline ReadLock readLock(page_number page) const 
    {
        return page;
    }
    
    inline WriteLock writeLock(page_number page) const
    {
        return page;
    }
};


}

#endif // __PAGANINI_PAGING_DUMMY_LOCKER_H__
