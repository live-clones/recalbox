#include "Thread.h"
#include <string>

int Thread::mCount = 0;

Thread::Thread()
  : mIsRunning(false), mIsDone(true)
{
  mCount++;
}

void Thread::Start(const std::string& name)
{
  mName = (!name.empty()) ? name : "thread " + std::to_string(mCount);

  mIsDone = false;
  pthread_create(&mHandle, nullptr, &Thread::StartThread, this);
  pthread_setname_np(mHandle, mName.c_str());
}

void Thread::Stop()
{
  mIsRunning = false;
  Break();
  void* dummy;
  pthread_join(mHandle, &dummy);
}

void* Thread::StartThread(void* thread_)
{
  Thread& thread = *((Thread*)(thread_));

  thread.mIsRunning = true;
  thread.BeforeRun();
  thread.Run();
  thread.AfterRun();
  thread.mIsDone = true;

  return nullptr;
}
