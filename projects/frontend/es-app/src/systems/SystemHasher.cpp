//
// Created by bkg2k on 02/03/23.
//

#include "SystemHasher.h"

SystemHasher::SystemHasher()
{
  Thread::Start("Hasher");
}

SystemHasher::~SystemHasher()
{
  Thread::Stop();
}

void SystemHasher::CheckMissingHashes(SystemData& system)
{
  class MissingHashes : public IParser
  {
    private:
      //! Parent reference
      SystemHasher& mHasher;
      //! Hashed rom count
      int mCount;
      //! Found flag
      bool mFound;

    public:
      explicit MissingHashes(SystemHasher& hasher)
        : mHasher(hasher)
        , mCount(0)
        , mFound(false)
      {}

      void Parse(FileData& game) override
      {
        if (!mHasher.IsRunning()) return;
        if (game.IsGame())
          if (game.Metadata().RomCrc32() == 0)
          {
            game.CalculateHash();
            mCount++;
            mFound = true;
          }
      }

      [[nodiscard]] int HashCount() const { return mCount; }
      [[nodiscard]] bool MissingHashesFound() const { return mFound; }
  } missingHashes(*this);

  if (RecalboxConf::Instance().GetNetplayEnabled())
    if (system.Descriptor().HasNetPlayCores())
    {
      DateTime start;
      system.MasterRoot().ParseAllItems(missingHashes);
      if (missingHashes.MissingHashesFound())
      { LOG(LogInfo) << "[SystemHasher] Calculated " << missingHashes.HashCount() << " missing hashes of " << system.FullName() << ". Took " << (DateTime() - start).TotalMilliseconds() << "ms."; }
    }
}

void SystemHasher::Run()
{
  // De-prioritize this thread to IDLE
  sched_param params { .sched_priority = 0 };
  if (pthread_setschedparam(pthread_self(), SCHED_IDLE, &params) == 0)
    { LOG(LogDebug) << "[SystemHasher] Hasher thread set to IDLE priority"; }
  else
    { LOG(LogError) << "[SystemHasher] Error setting IDLE priority to Hasher thread"; }

  // Loop
  while(IsRunning())
  {
    // Wait until a job is queued
    mSignal.WaitSignal();

    // Dequeue
    mLocker.Lock();
    SystemData* system = !mQueue.Empty() ? mQueue.Pop() : nullptr;
    mLocker.UnLock();

    if (system !=  nullptr)
    {
      { LOG(LogDebug) << "[SystemHasher] Start checking hash of " << system->FullName(); }
      CheckMissingHashes(*system);
    }
  }
}

void SystemHasher::Push(SystemData* system)
{
  mLocker.Lock();
  mQueue.Push(system);
  mLocker.UnLock();
  mSignal.Fire();
}

