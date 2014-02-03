

#include <stdint.h>

#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sched.h>

#include <string>

#include <libdfsutil/thread.h>

using namespace std;

// Add support for using Thread::Thread() to get the current thread or the static GetThread().

void EntryWrapper(Thread *t);

Thread::Thread()
{
    interrupted = false;
    cstate = Running;
    tname = "";
    tid = pthread_self(); // How do i fix this for the current thread
    attr = NULL;
}

Thread::Thread(const string &name)
{
    interrupted = false;
    cstate = Running;
    tname = name;
    tid = pthread_self();
    attr = NULL;
}

Thread::~Thread()
{
}

string Thread::getName()
{
    return tname;
}

void Thread::setName(const string &name)
{
    tname = name;
}

ThreadPriority Thread::getPriority()
{
    return NormalPriority;
}

void Thread::setPriority(ThreadPriority p)
{
    //pthread_setschedprio(tid,p);
}

int Thread::terminate()
{
    if (tid == pthread_self())
	pthread_exit(0);
    return pthread_kill(tid, SIGKILL);
}

void Thread::exit(void *retval)
{
    pthread_exit(retval);
}

void Thread::sleep(unsigned long secs)
{
    ::sleep(secs);
}

void Thread::usleep(unsigned long usecs)
{
    ::usleep(usecs);
}

void Thread::start()
{
    int ret = pthread_create(&tid,
			     attr,
			     (void *(*)(void *))EntryWrapper,
			     (void *)this);
    //pthread_attr_destroy(attr);
    if (ret != 0) {
	cstate = Running;
	return;
    }
    return;
}

bool Thread::wait(unsigned long time)
{
    // pthread_join
    return false;
}

void Thread::yield()
{
    sched_yield();
}

void EntryWrapper(Thread *t)
{
    t->run();
}

void
Thread::interrupt()
{
    interrupted = true;
}

bool
Thread::interruptionRequested()
{
    return interrupted;
}

threadid_t Thread::getID() {
#if defined(__APPLE__)
    return pthread_mach_thread_np(pthread_self());
#elif defined(__linux__)
    return ::syscall(SYS_gettid);
#elif defined(__FreeBSD__)
    long lwtid;
    thr_self(&lwtid);
    return lwtid;
#elif defined(__NetBSD__)
    return (uintptr_t)pthread_self();
#else
#error "Thread: platform not supported"
#endif
}

