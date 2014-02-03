

#include <libdfsutil/monitor.h>

Monitor::Monitor(Mutex &l)
    : lock(l)
{
    lock.lock();
}

Monitor::~Monitor()
{
    lock.unlock();
}

