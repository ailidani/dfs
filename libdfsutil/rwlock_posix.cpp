

#include <stdint.h>

#include <unistd.h>
#include <sys/types.h>

#include <string>
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
    pthread_rwlock_init(&lockHandle, NULL);
    lockNum = gLockNum++;
}

RWLock::~RWLock()
{
    pthread_rwlock_destroy(&lockHandle);
}

RWKey::sp RWLock::readLock()
{
#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    DLOG("%u readLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _checkLockOrdering();
#endif

    ////////////////////////////
    // Do the actual locking
    pthread_rwlock_rdlock(&lockHandle);

#if LOG_LOCKING == 1
    DLOG("%u success readLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _updateLocked();
#endif

    return RWKey::sp(new ReaderKey(this));
}

RWKey::sp RWLock::tryReadLock()
{
    NOT_IMPLEMENTED(false);
    if (pthread_rwlock_tryrdlock(&lockHandle) == 0) {
        return RWKey::sp(new ReaderKey(this));
    }
    return RWKey::sp();
}

void RWLock::readUnlock()
{
#if CHECK_LOCK_ORDER == 1
    gOrderMutex.lock();
#endif

    pthread_rwlock_unlock(&lockHandle);
    
#if CHECK_LOCK_ORDER == 1
    gLockedBy[lockNum] = Thread::TID_NOBODY;
    gOrderMutex.unlock();
#endif

#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    DLOG("%u readUnlock: %u", tid, lockNum);
#endif
}

RWKey::sp RWLock::writeLock()
{
#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    DLOG("%u writeLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _checkLockOrdering();
#endif

    ////////////////////////////
    // Do the actual locking
    pthread_rwlock_wrlock(&lockHandle);

#if LOG_LOCKING == 1
    DLOG("%u success writeLock: %u", tid, lockNum);
#endif

#if CHECK_LOCK_ORDER == 1
    _updateLocked();
#endif

    return RWKey::sp(new WriterKey(this));
}

RWKey::sp RWLock::tryWriteLock()
{
    NOT_IMPLEMENTED(false);
    if (pthread_rwlock_trywrlock(&lockHandle) == 0) {
        return RWKey::sp(new WriterKey(this));
    }
    return RWKey::sp();
}

void RWLock::writeUnlock()
{
#if CHECK_LOCK_ORDER == 1
    gOrderMutex.lock();
#endif

    pthread_rwlock_unlock(&lockHandle);
    
#if CHECK_LOCK_ORDER == 1
    gLockedBy[lockNum] = Thread::TID_NOBODY;
    gOrderMutex.unlock();
#endif

#if LOG_LOCKING == 1
    threadid_t tid = Thread::getID();
    DLOG("%u writeUnlock: %u", tid, lockNum);
#endif
}

