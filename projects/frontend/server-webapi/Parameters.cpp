//
// Created by bkg2k on 30/03/2020.
//

#include <cstring>
#include <utils/Strings.h>
#include <utils/Log.h>
#include "Parameters.h"

Parameters::Parameters(int argc, char** argv)
  : Parameters()
{
  bool error = false;
  for(int i = 1; i < argc; i++)
  {
    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--host") == 0))
    {
      if (i + 1 >= argc) error = true;
      else mIP = argv[++i];
    }
    else if ((strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--port") == 0))
    {
      if (i + 1 >= argc) error = true;
      else error = !Strings::ToInt(argv[++i], mPort);
    }
    else if ((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--threads") == 0))
    {
      if (i + 1 >= argc) error = true;
      else error = !Strings::ToInt(argv[++i], mThreads);
    }
    else if ((strcmp(argv[i], "-r") == 0) || (strcmp(argv[i], "--wwwroot") == 0))
    {
      if (i + 1 >= argc) error = true;
      else mWWWRoot = argv[++i];
    }
    else if ((strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--file") == 0))
    {
      if (i + 1 >= argc) error = true;
      else mDefaultFile = argv[++i];
    }
    else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0) ||
             (strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--debug") == 0))
    {
      mDebug = true;
    }
    else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
    {
      error = true;
    }
    else
    {
      LOG(LogError) << "Unknown parameter " << argv[i];
      error = true;
    }
  }

  if (error)
  {
    Help();
    exit(1);
  }
}

void Parameters::Help()
{
  puts("Usage: webapi-server [options]\n"
       "  options:\n"
       "    -h, --host    <host/ip> : bind the server to the given host/ip. default value: 0.0.0.0\n"
       "    -p, --port    <portnum> : bind the server to the port P. default value: 20666\n"
       "    -t, --threads <num>     : maximum simultaneous processing threads. default: 5\n"
       "    -r, --wwwroot <path>    : www root folder. Default: /recalbox/web/manager-v3\n"
       "    -f, --file    <file>    : www root folder default file. Default: index.html\n"
       "    -d, --debug             : enable debug logs.\n"
       );
}

void Parameters::LogConfig()
{
  LOG(LogInfo) << "Parameters:";
  LOG(LogInfo) << "  Interface    : " << mIP;
  LOG(LogInfo) << "  Port         : " << mPort;
  LOG(LogInfo) << "  Threads      : " << mThreads;
  LOG(LogInfo) << "  Root folder  : " << mWWWRoot;
  LOG(LogInfo) << "  Default file : " << mDefaultFile;
  LOG(LogInfo) << "  Debug logs   : " << mDebug;
}
