#include "Mutex.h"

Mutex::Mutex()
{
  pthread_mutexattr_t MutexAttr;
  pthread_mutexattr_init(&MutexAttr);
  pthread_mutexattr_settype(&MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(&mMutex, &MutexAttr);
  pthread_cond_init(&mCondition, nullptr);
}

Mutex::~Mutex()
{
  pthread_cond_destroy(&mCondition);
  if(pthread_mutex_destroy(&mMutex) != 0)
  {
    UnLock();
    pthread_mutex_destroy(&mMutex);
  }
}

bool Mutex::Lock()
{
  if(pthread_mutex_lock(&mMutex) == 0) return true;
  return false;
}

bool Mutex::UnLock()
{
  if(pthread_mutex_unlock(&mMutex) == 0) return true;
  return false;
}

bool Mutex::Signal()
{
  pthread_cond_signal(&mCondition);
  return true;
}

bool Mutex::WaitSignal()
{
  Lock();
  pthread_cond_wait(&mCondition, &mMutex);
  UnLock();
  return true;
}

bool Mutex::WaitSignal(int milliseconds)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  if( milliseconds>1000 )
  {
    ts.tv_sec += milliseconds/1000;
    milliseconds %= 1000;
  }
  ts.tv_nsec += (long)milliseconds * 1000000L;
  pthread_cond_timedwait(&mCondition, &mMutex, &ts);
  return true;
}
