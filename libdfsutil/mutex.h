
#ifndef libdfsutil_mutex_h
#define libdfsutil_mutex_h


#if defined(__APPLE__) || defined(__linux__) || defined(__FreeBSD__) \
    || defined(__NetBSD__)
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "UNSUPPORTED OS"
#endif

class Mutex
{
public:
    Mutex();
    virtual ~Mutex();
    void lock();
    void unlock();
    bool tryLock();
    // bool Locked();
private:
#if defined(__APPLE__) || defined(__linux__) || defined(__FreeBSD__) \
    || defined(__NetBSD__)
    pthread_mutex_t lockHandle;
#elif defined(_WIN32)
    CRITICAL_SECTION lockHandle;
#else
#error "UNSUPPORTED OS"
#endif
};

#endif
