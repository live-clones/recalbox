#include "CommandThread.h"
#include "utils/Log.h"
#include "views/ViewController.h"
#include "systems/SystemManager.h"
#include <utils/Strings.h>
#include <netinet/in.h>

CommandThread::CommandThread(SystemManager& systemManager)
  : mSystemManager(systemManager),
    mSocket(-1),
    mEvent(this)
{
  Thread::Start("CommandThread");
}

CommandThread::~CommandThread()
{
  Thread::Stop();
}

void CommandThread::Run()
{
  try
  {
    sleep(5);

    { LOG(LogInfo) << "[Command] CommandThread started"; }

    if (OpenUDP())
      while (IsRunning())
      {
        std::string frame = ReadUDP();
        std::vector<std::string> tokens = Strings::Split(frame, '|');
        if (tokens.size() < 3) continue;

        std::string command = tokens[0];
        std::string systemName = tokens[1];
        std::string gameName = tokens[2];

        // Check that the command is valid. Easy way as there is just 1 for now
        if (command != "START") { LOG(LogError) << "[Command] Wrong network command " << tokens[0]; continue; }

        // Get and check system
        SystemData *system = mSystemManager.SystemByName(systemName);
        if (system == nullptr) { LOG(LogError) << "[Command] Invalid system on network command: " << systemName; continue; }

        // Get and check game
        FileData* result = system->MasterRoot().LookupGame(gameName, FileData::SearchAttributes::ByNameWithExt);
        if (result != nullptr) { LOG(LogInfo) << "[Command] Starting game " << gameName << " for system " << systemName; mEvent.Call(result); }
        else { LOG(LogError) << "[Command] Couldn't find game " << gameName << " for system " << systemName; }
      }
  }
  catch(std::exception& ex)
  {
    { LOG(LogError) << "[Command] CommandThread thread crashed."; }
    { LOG(LogError) << "[Command] Exception: " << ex.what(); }
  }
}

std::string CommandThread::ReadUDP() const
{
  sockaddr_in si_other = {};
  char buffer[2048];
  long size, slen = sizeof(si_other);
  if ((size = recvfrom(mSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &si_other, (socklen_t*)&slen)) > 0)
    return std::string(buffer, size);
  return std::string();
}

bool CommandThread::OpenUDP()
{
  while(IsRunning())
  {
    // Create an UDP socket
    if ((mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0)
    {
      // zero out the structure
      sockaddr_in si_me = {};
      memset((char*) &si_me, 0, sizeof(si_me));
      si_me.sin_family = AF_INET;
      si_me.sin_port = htons(sPort);
      si_me.sin_addr.s_addr = htonl(INADDR_ANY);

      //bind socket to port
      if (bind(mSocket, (struct sockaddr*) &si_me, sizeof(si_me)) >= 0)
        return true;
    }
    // Wait a bit before the next try
    sleep(1);
  }

  return false;
}

void CommandThread::ReceiveSyncCallback(const SDL_Event& event)
{
  // Get game
  FileData* game = (FileData*)event.user.data1;

  ViewController::Instance().WakeUp();
  ViewController::Instance().Launch(game, GameLinkedData(), Vector3f());
}

