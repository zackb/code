#ifndef __ZB_LOCK
#define __ZB_LOCK 1

#include "log.h"

class mutex {
        pthread_mutex_t _mutex;
    public:
        mutex() { }
        ~mutex() { unlock(); }
        void lock() { Log::debug("Lock", NULL); pthread_mutex_lock(&_mutex); }
        void unlock()   { Log::debug("Unlock", NULL); pthread_mutex_unlock(&_mutex); }
};

class Lock {
    mutex &m;
    public:
        Lock(mutex &x) : m(x) { m.lock(); }
        ~Lock() { m.unlock(); }
};

#endif
