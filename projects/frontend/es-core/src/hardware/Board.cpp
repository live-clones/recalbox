#include <hardware/Board.h>
#include <cstring>
#include <utils/Log.h>
#include <utils/Files.h>
#include <input/InputCompactEvent.h>
#include <hardware/boards/odroidadvancego2/OdroidAdvanceGo2Board.h>
#include <sys/utsname.h>
#include <hardware/boards/NullBoard.h>
#include <hardware/boards/pc/PcComputers.h>
#include "hardware/crt/CrtAdapterDetector.h"
#include "hardware/crt/CrtNull.h"
#include <hardware/boards/pi400/Pi400Board.h>

Board::Board(IHardwareNotifications& notificationInterface)
  : StaticLifeCycleControler("Board")
  , mType(BoardType::UndetectedYet)
  , mMemory(0)
  , mSender(notificationInterface)
  , mBoard(GetBoardInterface(mSender))
  , mCrtBoard(GetCrtBoard())
{
}

IBoardInterface& Board::GetBoardInterface(HardwareMessageSender& messageSender)
{
  BoardType model = GetBoardType();
  switch(model)
  {
    case BoardType::OdroidAdvanceGo:
    {
      { LOG(LogInfo) << "[Hardware] Odroid Advance Go 1/2 detected."; }
      return *(new OdroidAdvanceGo2Board(messageSender, model));
    }
    case BoardType::OdroidAdvanceGoSuper:
    {
      { LOG(LogInfo) << "[Hardware] Odroid Advance Go Super detected."; }
      return *(new OdroidAdvanceGo2Board(messageSender, model));
    }
    case BoardType::PCx86:
    case BoardType::PCx64:
    {
      { LOG(LogInfo) << "[Hardware] x86 or x64 PC detected."; }
      return *(new PcComputers(messageSender));
    }
    case BoardType::UndetectedYet: { LOG(LogInfo) << "[Hardware] Undetected hardware."; break; }
    case BoardType::Unknown: { LOG(LogInfo) << "[Hardware] Unknown hardware."; break; }
    case BoardType::Pi0: { LOG(LogInfo) << "[Hardware] Pi zero detected."; break; }
    case BoardType::Pi02: { LOG(LogInfo) << "[Hardware] Pi zero 2 detected."; break; }
    case BoardType::Pi1: { LOG(LogInfo) << "[Hardware] Pi 1 detected."; break; }
    case BoardType::Pi2: { LOG(LogInfo) << "[Hardware] Pi 2 detected."; break; }
    case BoardType::Pi3: { LOG(LogInfo) << "[Hardware] Pi 3 detected."; break; }
    case BoardType::Pi3plus: { LOG(LogInfo) << "[Hardware] Pi 3B+ detected."; break; }
    case BoardType::Pi4: { LOG(LogInfo) << "[Hardware] Pi 4 detected."; break; }
    case BoardType::Pi400:
    {
      { LOG(LogInfo) << "[Hardware] Pi 400 detected."; }
      return *(new Pi400Board(messageSender));
    }
    case BoardType::UnknownPi: { LOG(LogInfo) << "[Hardware] Unknown raspberry pi detected."; break; }
    default: break;
  }

  { LOG(LogInfo) << "[Hardware] Default hardware interface created."; }
  return *(new NullBoard(messageSender));
}

int Board::GetPiMemory(unsigned int revision)
{
  int  memorySize     = ((int)revision >> 20) & 0x7;;

  switch(memorySize)
  {
    case 0: return 256;
    case 1: return 512;
    case 2: return 1024;
    case 3: return 2048;
    case 4: return 4096;
    default: break;
  }

  return 8192;
}

BoardType Board::GetPiModel(unsigned int revision)
{
  // Split - uuuuuuuuFMMMCCCCPPPPTTTTTTTTRRRR
  bool newGeneration  = ((revision >> 23) & 1) != 0;
  int  memorySize     = ((int)revision >> 20) & 0x7; (void)memorySize;
  int  manufacturer   = ((int)revision >> 16) & 0xF; (void)manufacturer;
  int  processor      = ((int)revision >> 12) & 0xF; (void)processor;
  int  model          = ((int)revision >>  4) & 0xFF;
  int  revisionNumber = ((int)revision >>  0) & 0xF; (void)revisionNumber;

  // Old revision numbering
  if (!newGeneration)
    return BoardType::Pi1;

  // New models
  switch ((RaspberryModel)model)
  {
    case RaspberryModel::Zero:
    case RaspberryModel::ZeroW: return BoardType::Pi0;
    case RaspberryModel::Zero2: return BoardType::Pi02;
    case RaspberryModel::OneA:
    case RaspberryModel::OneAPlus:
    case RaspberryModel::OneB:
    case RaspberryModel::OneBPlus:
    case RaspberryModel::OneCM1: return BoardType::Pi1;
    case RaspberryModel::TwoB: return BoardType::Pi2;
    case RaspberryModel::TreeB:
    case RaspberryModel::TreeCM3: return BoardType::Pi3;
    case RaspberryModel::TreeBPlus:
    case RaspberryModel::TreeCM3Plus:
    case RaspberryModel::TreeAPlus: return BoardType::Pi3plus;
    case RaspberryModel::FourB:
    case RaspberryModel::FourCM4: return BoardType::Pi4;
    case RaspberryModel::FourHundred: return BoardType::Pi400;
    case RaspberryModel::Alpha:
    default: break;
  }

  return BoardType::UnknownPi;
}

#define CPU_INFO_FILE   "/proc/cpuinfo"
#define REVISION_STRING "Revision"
#define HARDWARE_STRING "Hardware"

#define SizeLitteral(x) (sizeof(x) - 1)

int Board::TotalMemory()
{
  if (mType == BoardType::UndetectedYet) GetBoardType();
  return mMemory;
}

BoardType Board::GetBoardType()
{
  if (mType != BoardType::UndetectedYet) return mType;

  // Try uname (for PC)
  utsname uName {};
  memset(&uName, 0, sizeof(uName));
  uname(&uName);
  std::string machine(uName.machine);
  { LOG(LogDebug) << "[Hardware] Machine identifier: '" << machine << '\''; }
  if (machine == "i386") return mType = BoardType::PCx86;
  if (machine == "i686") return mType = BoardType::PCx86;
  if (machine == "x86_64") return mType = BoardType::PCx64;

  // Then try CPU info
  std::string hardware;
  std::string revision;
  mType = BoardType::Unknown;

  FILE* f = fopen(CPU_INFO_FILE, "r");
  if (f != nullptr)
  {
    char line[1024];
    std::string str; // Declared before loop to keep memory allocated
    while (fgets(line, sizeof(line) - 1, f) != nullptr)
    {
      // Raspberry pi
      if (strncmp(line, HARDWARE_STRING, SizeLitteral(HARDWARE_STRING)) == 0)
      {
        char* colon = strchr(line, ':');
        if (colon != nullptr)
        {
          hardware = colon + 2;
          hardware.erase(hardware.find_last_not_of(" \t\r\n") + 1);
          { LOG(LogInfo) << "[Hardware] Hardware " << hardware; }
        }
      }
      if (strncmp(line, REVISION_STRING, SizeLitteral(REVISION_STRING)) == 0)
      {
        char* colon = strchr(line, ':');
        if (colon != nullptr)
        {
          revision = colon + 2;
          revision.erase(revision.find_last_not_of(" \t\r\n") + 1);
          unsigned int irevision = (int) strtol(colon + 2, nullptr, 16); // Convert hexa revision

          if ((hardware == "BCM2835") || (hardware == "BCM2711"))
          {
            { LOG(LogInfo) << "[Hardware] Pi revision " << revision; }
            mType = GetPiModel(irevision);
            mMemory = GetPiMemory(irevision);
          }
          if (hardware == "Hardkernel ODROID-GO3")
          {
            { LOG(LogInfo) << "[Hardware] Odroid Advance Go Super revision " << revision; }
            mType = BoardType::OdroidAdvanceGoSuper;
          }
          if ((hardware == "Hardkernel ODROID-GO2") || (hardware == "Hardkernel ODROID-GO1") || (hardware == "Hardkernel ODROID-GO"))
          {
            { LOG(LogInfo) << "[Hardware] Odroid Advance Go 1/2 revision " << revision; }
            mType = BoardType::OdroidAdvanceGo;
          }
        }
      }
    }
    fclose(f);
  }

  return mType;
}

ICrtInterface& Board::GetCrtBoard()
{
  return *(CanHaveCRTBoard() ? CrtAdapterDetector::CreateCrtBoard() : new CrtNull(false));
}

bool Board::CanHaveCRTBoard()
{
  #ifdef DEBUG
  return true;
  #else
  switch(GetBoardType())
  {
    case BoardType::Pi02:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400: return true;
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::UnknownPi:
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default: break;
  }

  return false;
  #endif
}
