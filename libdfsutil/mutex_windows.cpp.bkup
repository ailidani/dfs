

#include <windows.h>

#include <libdfsutil/mutex.h>

Mutex::Mutex()
{
    InitializeCriticalSection(&lockHandle);
}

Mutex::~Mutex()
{
    DeleteCriticalSection(&lockHandle);
}

void Mutex::lock()
{
    EnterCriticalSection(&lockHandle);
}

void Mutex::unlock()
{
    LeaveCriticalSection(&lockHandle);
}

bool Mutex::tryLock()
{
    return (TryEnterCriticalSection(&lockHandle) != 0);
}

