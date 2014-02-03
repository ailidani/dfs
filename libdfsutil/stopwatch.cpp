

#include <stdint.h>

#if defined(__APPLE__) || defined(__linux__) || defined(__FreeBSD__) \
    || defined(__NetBSD__)
#include <sys/types.h>
#include <sys/time.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "UNSUPPORTED OS"
#endif

#include <libdfsutil/stopwatch.h>

Stopwatch::Stopwatch()
{
    startTime = 0LL;
    elapsedTime = 0LL;
    running = false;
}

Stopwatch::~Stopwatch()
{
}

void Stopwatch::start()
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    startTime = tv.tv_sec * 1000000 + tv.tv_usec;
    running = true;
}

void Stopwatch::stop()
{
    uint64_t curTime;
    struct timeval tv;

    gettimeofday(&tv, 0);
    curTime = tv.tv_sec * 1000000 + tv.tv_usec;
    elapsedTime += curTime - startTime;
    running = false;
}

void Stopwatch::reset()
{
    startTime = 0LL;
    elapsedTime = 0LL;
}

uint64_t Stopwatch::getElapsedTime()
{
    uint64_t curTime;
    struct timeval tv;

    gettimeofday(&tv, 0);
    curTime = tv.tv_sec * 1000000 + tv.tv_usec;

    if (running == true)
    {
        return elapsedTime + (curTime - startTime);
    } else {
        return elapsedTime;
    }
}

