

#ifndef libdfsutil_monitor_h
#define libdfsutil_monitor_h

#include "mutex.h"

class Monitor
{
public:
    Monitor(Mutex &l);
    virtual ~Monitor();
private:
    Mutex &lock;
};

#endif

