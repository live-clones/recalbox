//
// Created by bkg2k on 30/03/2020.
//
#pragma once

#include "IRouter.h"
#include "Parameters.h"
#include <pistache/endpoint.h>
#include <pistache/net.h>

class Server
{
  private:
    Pistache::Address mAddress;
    Http::Endpoint mServer;

  public:
    /*!
     * @brief Default Constructor
     */
    Server(const Parameters& param, IRouter* router);

    /*!
     * @brief Run the server
     */
    void Run();

    /*!
     * @brief Stop the server
     */
    void Cancel();
};
