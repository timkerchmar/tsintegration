#ifndef TSTHREAD_H
#define TSTHREAD_H

#include <string>

#include <sys/types.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

class TSMutex
{
public:
    TSMutex();
#ifdef WIN32
    CRITICAL_SECTION handle;
#else
    pthread_mutex_t handle;
#endif
    void lock();
    void unlock();
};

class TSConditionVariable
{
public:
    TSConditionVariable();
    
#ifdef WIN32
    CONDITION_VARIABLE handle;
#else
    pthread_cond_t handle;
#endif
    void wait(TSMutex &mutex);
    void notify();
};

class TSThread
{
public:
    void start();
    
    virtual void run() = 0;
    virtual std::string name() = 0;

#ifdef WIN32
	HANDLE handle;
#else
	pthread_t handle;
#endif
};

#endif //TSTHREAD_H
