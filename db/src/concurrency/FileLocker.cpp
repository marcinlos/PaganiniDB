#include "config.h"
#include <paganini/concurrency/FileLocker.h>


namespace paganini
{
namespace concurrency
{


FileLocker::FileLocker(int descriptor): descriptor_(descriptor)
{
}


FileLocker::~FileLocker()
{
    unlock_all_();
}


void FileLocker::setFile(int descriptor)
{
    unlock_all_();
    descriptor_ = descriptor;
}


void FileLocker::lock_aux_(page_number page, short type, int cmd)
{
    struct flock lock;
    lock.l_type = type;
    lock.l_start = PAGE_SIZE * page;
    lock.l_whence = SEEK_SET;
    lock.l_len = PAGE_SIZE;
    if (fcntl(descriptor_, cmd, &lock) < 0)
    {
        throw SystemError(util::format("Failed fcntl call for page {} "
            "(type={},cmd={})", page, type, cmd), errno);
    }
}


FileLocker::LockInfo& FileLocker::findLockInfo_(page_number page)
{
    auto i = locks_.find(page);
    if (i == locks_.end())
        i = locks_.insert({page, {0, 0}}).first;
    return i->second;
}


void FileLocker::pageLockRead(page_number page)
{
    LockInfo& info = findLockInfo_(page);
    if (info.read_count == 0)
    {
        lock_read_(page);
    }
    ++ info.read_count;
}


void FileLocker::pageLockWrite(page_number page)
{
    LockInfo& info = findLockInfo_(page);
    if (info.write_count == 0)
    {
        lock_write_(page);
    }
    ++ info.write_count;
}


void FileLocker::lock_read_(page_number page)
{
    lock_aux_(page, F_RDLCK, F_SETLKW);
}


void FileLocker::lock_write_(page_number page)
{
    lock_aux_(page, F_WRLCK, F_SETLKW);
}


void FileLocker::unlock_(page_number page)
{
    lock_aux_(page, F_UNLCK, F_SETLKW);
}


void FileLocker::unlock_all_()
{
    for (const auto& lock: locks_)
        unlock_(lock.first);
    locks_.clear();
}


void FileLocker::pageUnlockRead(page_number page)
{
    LockInfo& info = findLockInfo_(page);
    if (info.read_count > 0)
    {
        if (info.read_count == 1)
        {
            unlock_(page);
        }
        -- info.read_count;
    }
    else 
    {
        throw std::logic_error(util::format("Trying to unlock page {}, "
            "which is not locked for reading"));
    }
}


void FileLocker::pageUnlockWrite(page_number page)
{
    LockInfo& info = findLockInfo_(page);
    if (info.write_count > 0)
    {
        if (info.write_count == 1)
        {
            if (info.read_count == 0)
                unlock_(page);
            else 
                lock_read_(page);
        }
        -- info.write_count;  
    }
    else
    {
        throw std::logic_error(util::format("Trying to unlock page {}, "
            "which is not locked for writing"));
    }
}


FileLocker::ReadLock FileLocker::readLock(page_number page)
{
    return { *this, page };
}


FileLocker::WriteLock FileLocker::writeLock(page_number page)
{
    return { *this, page };
}


} // concurrency
} // paganini

