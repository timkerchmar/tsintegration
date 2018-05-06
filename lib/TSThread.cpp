#include "TSThread.h"
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef ANDROID
#include <sys/prctl.h>
#endif

TSMutex::TSMutex()
{
#ifdef WIN32
    InitializeCriticalSection(&handle);
#else
    handle = PTHREAD_MUTEX_INITIALIZER;
#endif
}

void TSMutex::lock()
{
#ifdef WIN32
	EnterCriticalSection(&handle);
#else
	pthread_mutex_lock(&handle);
#endif
}

void TSMutex::unlock() {
#ifdef WIN32
	LeaveCriticalSection(&handle);
#else
	pthread_mutex_unlock(&handle);
#endif
}

TSConditionVariable::TSConditionVariable()
{
#ifdef WIN32
    InitializeConditionVariable(&handle);
#else
    handle = PTHREAD_COND_INITIALIZER;
#endif
}

void TSConditionVariable::wait(TSMutex& mutex)
{
#ifdef WIN32
    SleepConditionVariableCS(&handle, &mutex.handle, INFINITE);
#else
    pthread_cond_wait(&handle, &mutex.handle);
#endif
}

void TSConditionVariable::notify()
{
#ifdef WIN32
    WakeConditionVariable(&handle);
#else
    pthread_cond_signal(&handle);
#endif
}

#ifdef WIN32
DWORD WINAPI
#else
void* 
#endif
TSThreadMethod(void* threadObject)
{
    TSThread *thread = (TSThread *)threadObject;
    
#ifndef WIN32
    pthread_setname_np(
#ifdef ANDROID
                       thread->handle,
#endif
                       thread->name().c_str());
#endif
#ifdef ANDROID
    prctl(PR_SET_NAME, thread->name().c_str(), 0, 0, 0);
#endif

    thread->run();
    
    return NULL;
}

void TSThread::start()
{
#ifdef WIN32
	handle =
	CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		TSThreadMethod,       // thread function name
		this,          // argument to thread function 
		0,                      // use default creation flags 
		NULL);// might need to put handle here instead
#else
    pthread_create(&handle, NULL, TSThreadMethod, this);
    pthread_detach(handle);
#endif
}
