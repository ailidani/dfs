

#ifndef libdfsutil_rwlock_h
#define libdfsutil_rwlock_h

#include <stdint.h>

#if defined(__APPLE__) || defined(__linux__) || defined(__FreeBSD__) \
    || defined(__NetBSD__)
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "UNSUPPORTED OS"
#endif

#include <vector>
#include <map>

#include "thread.h"
#include "mutex.h"

class RWLock;
struct RWKey {
    typedef std::shared_ptr<RWKey> sp;
    virtual ~RWKey() { };
};
struct ReaderKey : public RWKey {
protected:
    ReaderKey(RWLock *l = NULL);
public:
    virtual ~ReaderKey();
private:
    RWLock *lock;
    friend class RWLock;
};
struct WriterKey : public RWKey {
protected:
    WriterKey(RWLock *l = NULL);
public:
    virtual ~WriterKey();
private:
    RWLock *lock;
    friend class RWLock;
};

class RWLock
{
public:
    RWLock();
    ~RWLock();
    RWKey::sp readLock();
    RWKey::sp tryReadLock();
    void readUnlock();
    RWKey::sp writeLock();
    RWKey::sp tryWriteLock();
    void writeUnlock();
    // bool locked();

    typedef std::vector<uint32_t> LockOrderVector;
    static void setLockOrder(const LockOrderVector &order);
    ///^ lock orderings cannot be changed once set

    uint32_t lockNum;
private:
    static Mutex gOrderMutex;
    static uint32_t gLockNum;
    static std::map<uint32_t, size_t> gLockNumToOrdering;
    static std::vector<LockOrderVector> gLockOrderings;
    static std::map<uint32_t, threadid_t> gLockedBy;

    void _checkLockOrdering();
    void _updateLocked();

#if defined(__APPLE__) || defined(__linux__) || defined(__FreeBSD__) \
    || defined(__NetBSD__)
    pthread_rwlock_t lockHandle;
#elif defined(_WIN32)
    SRWLOCK lockHandle;
#else
#error "UNSUPPORTED OS"
#endif
};

#endif

