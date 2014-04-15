

#include <stdint.h>

#include <windows.h>

#include <string>
#include <vector>
#include <map>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/rwlock.h>
#include <libdfsutil/thread.h>

#define LOG_LOCKING 0

#ifdef DEBUG
#define CHECK_LOCK_ORDER 1
#else
#define CHECK_LOCK_ORDER 0
#endif

RWLock::RWLock()
    : lockHandle()
{
    InitializeSRWLock(&lockHandle);
    lockNum = gLockNum++;
}

RWLock::~RWLock()
{
}

RWKey::sp RWLock::readLock()
{
#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    LOG("%u readLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _checkLockOrdering();
#endif

    ////////////////////////////
    // Do the actual locking
    AcquireSRWLockShared(&lockHandle);

#if LOG_LOCKING == 1
    LOG("%u success readLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _updateLocked();
#endif

    return RWKey::sp(new ReaderKey(this));
}

RWKey::sp RWLock::tryReadLock()
{
    NOT_IMPLEMENTED(false);
    if (TryAcquireSRWLockShared(&lockHandle) != 0) {
        return RWKey::sp(new ReaderKey(this));
    }
    return RWKey::sp();
}

RWKey::sp RWLock::writeLock()
{
#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    LOG("%u writeLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _checkLockOrdering();
#endif

    ////////////////////////////
    // Do the actual locking
    AcquireSRWLockExclusive(&lockHandle);

#if LOG_LOCKING == 1
    LOG("%u success writeLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _updateLocked();
#endif

    return RWKey::sp(new WriterKey(this));
}

RWKey::sp RWLock::tryWriteLock()
{
    NOT_IMPLEMENTED(false);
    if (TryAcquireSRWLockExclusive(&lockHandle) != 0) {
        return RWKey::sp(new WriterKey(this));
    }
    return RWKey::sp();
}

void RWLock::unlock()
{
#if CHECK_LOCK_ORDER == 1
    gOrderMutex.lock();
#endif

    // ReleaseSRWLockShared or ReleaseSRWLockExclusive

#if CHECK_LOCK_ORDER == 1
    gLockedBy[lockNum] = Thread::TID_NOBODY;
    gOrderMutex.unlock();
#endif

#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    LOG("%u unlock: %u", tid, lockNum);
#endif
}

