

#ifndef libdfsutil_stopwatch_h
#define libdfsutil_stopwatch_h

class Stopwatch
{
public:
    Stopwatch();
    ~Stopwatch();
    void start();
    void stop();
    void reset();
    uint64_t getElapsedTime();
private:
    uint64_t startTime;
    uint64_t elapsedTime;
    bool running;
};

#endif

