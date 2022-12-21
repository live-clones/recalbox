//
// Created by bkg2k on 30/03/2020.
//

#include <cassert>
#include "Server.h"

Server::Server(const Parameters& param, IRouter* router)
  : mAddress(param.IP(), Pistache::Port(param.Port())),
    mServer(mAddress)
{
  assert(router != nullptr);

  auto opts = Http::Endpoint::options().threads(param.Threads());
  mServer.init(opts);
  mServer.setHandler(router->Handler());
}

void Server::Run()
{
  for(;;)
    try
    {
      LOG(LogInfo) << "Server up!";
      mServer.serve();
      break;
    }
    catch(std::exception& ex)
    {
      LOG(LogError) << "Error running server! Retrying in 5s... (Exception: " << ex.what() << ')';
      sleep(5);
    }

  mServer.shutdown();
  LOG(LogInfo) << "Server down.";
}

void Server::Cancel()
{
  LOG(LogInfo) << "Server interrupted!";
  mServer.shutdown();
}
