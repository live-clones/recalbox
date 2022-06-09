#include "Thread.h"
#include <string>
#include <utils/Log.h>
#include <cstring>

Thread::Thread()
  : mName(),
    mHandle(0),
    mId(0),
    mIsRunning(false),
    mIsDone(false)
{
  static int sTotalCount = 0;
  memset(mName, 0, sizeof(mName));
  std::string strName = "thread " + std::to_string(++sTotalCount);
  int len = (int)strName.size() + 1;
  if (len > (int)sizeof(mName)) len = (int)sizeof(mName);
  strncpy(mName, strName.c_str(), len);
}

Thread::~Thread()
{
  Stop();
}

void Thread::Start(const std::string& name)
{
  Stop();

  if (!name.empty())
    strncpy(mName, name.c_str(), sizeof(mName) - 1);

  mIsDone = false;
  mIsRunning = true;
  int err = pthread_create(&mHandle, nullptr, &Thread::StartThread, this);
  if (err != 0) { LOG(LogError) << "[Thread] Error running thread " << mName; }
  pthread_setname_np(mHandle, mName);
}

void Thread::Stop()
{
  mIsRunning = false;
  if (mHandle != 0 && SelfId() != mId)
  {
    Break();
    void* dummy = nullptr;
    pthread_join(mHandle, &dummy);
    mHandle = 0;
  }
  mIsDone = true;
}

void Thread::Join()
{
  if (mHandle != 0 && SelfId() != mId)
  {
    void* dummy = nullptr;
    pthread_join(mHandle, &dummy);
    mHandle = 0;
  }
  mIsRunning = false;
  mIsDone = true;
}

void* Thread::StartThread(void* thread_)
{
  Thread& thread = *((Thread*)(thread_));
  thread.mId = SelfId();

  { LOG(LogDebug) << "[Thread] Thread " << thread.mName << " started!"; }

  thread.BeforeRun();
  thread.Run();
  thread.AfterRun();

  { LOG(LogDebug) << "[Thread] Thread " << thread.mName << " exited!"; }

  thread.mId = 0;
  thread.mIsRunning = false;
  thread.mIsDone = true;
  return nullptr;
}
