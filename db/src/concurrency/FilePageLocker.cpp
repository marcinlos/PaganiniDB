#include "config.h"
#include <paganini/concurrency/FilePageLocker.h>
#include <paganini/concurrency/SystemError.h>
#include <paganini/util/format.h>
#include <fcntl.h>


namespace paganini
{
namespace concurrency
{


FilePageLocker::FilePageLocker(int descriptor): descriptor_(descriptor)
{
}


FilePageLocker::~FilePageLocker()
{
    unlock_all_();
}


void FilePageLocker::setFile(int descriptor)
{
    unlock_all_();
    descriptor_ = descriptor;
}


void FilePageLocker::lock_aux_(page_number page, short type, int cmd)
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


FilePageLocker::LockInfo& FilePageLocker::find_lock_info_(page_number page)
{
    auto i = locks_.find(page);
    if (i == locks_.end())
        i = locks_.insert({page, {0, 0}}).first;
    return i->second;
}


void FilePageLocker::pageLockRead(page_number page)
{
    LockInfo& info = find_lock_info_(page);
    // Jesli nie ma zadnych lockow zadnego typu, lockujemy do czytania.
    // Jesli sa locki typu write nic nie robimy, lock_read_ spowodowalby
    // oddanie locka typu write.
    if (info.read_count == 0 && info.write_count == 0)
    {
        lock_read_(page);
    }
    ++ info.read_count;
}


void FilePageLocker::pageLockWrite(page_number page)
{
    LockInfo& info = find_lock_info_(page);
    if (info.write_count == 0)
    {
        lock_write_(page);
    }
    ++ info.write_count;
}


void FilePageLocker::lock_read_(page_number page)
{
    lock_aux_(page, F_RDLCK, F_SETLKW);
}


void FilePageLocker::lock_write_(page_number page)
{
    lock_aux_(page, F_WRLCK, F_SETLKW);
}


void FilePageLocker::unlock_(page_number page)
{
    lock_aux_(page, F_UNLCK, F_SETLKW);
    // Natychmiastowe usuwanie moze byc suboptymalne
    locks_.erase(page);
}


void FilePageLocker::unlock_all_()
{
    for (const auto& lock: locks_)
        unlock_(lock.first);
    locks_.clear();
}


void FilePageLocker::pageUnlockRead(page_number page)
{
    LockInfo& info = find_lock_info_(page);
    if (info.read_count > 0)
    {
        -- info.read_count;
        // Odblokowujemy tylko jesli to byl ostatni lock jakiegokolwiek typu
        if (info.read_count == 0 && info.write_count == 0)
        {
            unlock_(page);
        }            
    }
    else 
    {
        throw std::logic_error(util::format("Trying to unlock page {}, "
            "which is not locked for reading", page));
    }
}


void FilePageLocker::pageUnlockWrite(page_number page)
{
    LockInfo& info = find_lock_info_(page);
    if (info.write_count > 0)
    {
        -- info.write_count; 
        // Bezposrednie dzialania podejmujemy tylko jedli to byl ostatni lock
        // typu write - albo zwalniamy calkiem, albo downgradujemy do read
        if (info.write_count == 0)
        {
            if (info.read_count == 0)
                unlock_(page);
            else 
                lock_read_(page);
        }
    }
    else
    {
        throw std::logic_error(util::format("Trying to unlock page {}, "
            "which is not locked for writing", page));
    }
}


} // concurrency
} // paganini

