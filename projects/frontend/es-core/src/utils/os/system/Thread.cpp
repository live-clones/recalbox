#include "Thread.h"
#include <string>
#include <utils/Log.h>
#include <cstring>

Thread::Thread()
  : mName(),
    mHandle(0),
    mIsRunning(false),
    mIsDone(false)
{
  static int __TotalCount__ = 0;
  std::string strName = "thread " + std::to_string(++__TotalCount__);
  strncpy(mName, strName.c_str(), sizeof(mName));
}

Thread::~Thread()
{
  Stop();
}

void Thread::Start(const std::string& name)
{
  Stop();

  if (!name.empty())
    strncpy(mName, name.c_str(), sizeof(mName));
  LOG(LogDebug) << "Start thread! " << name;

  mIsDone = false;
  mIsRunning = true;
  int err = pthread_create(&mHandle, nullptr, &Thread::StartThread, this);
  if (err != 0) LOG(LogError) << "Error running thread " << mName;
  pthread_setname_np(mHandle, mName);
}

void Thread::Stop()
{
  mIsRunning = false;
  if (mHandle != 0)
  {
    Break();
    void* dummy;
    pthread_join(mHandle, &dummy);
    mHandle = 0;
  }
  mIsDone = true;
}

void* Thread::StartThread(void* thread_)
{
  Thread& thread = *((Thread*)(thread_));

  thread.BeforeRun();
  thread.Run();
  thread.AfterRun();

  LOG(LogDebug) << "Thread " << thread.mName << " exited!";

  return nullptr;
}
