//
// Created by digitalLumberjack on 08/05/2022.
//

#include "PatronInfo.h"
#include "utils/network/Http.h"
#include "utils/Log.h"
#include <rapidjson/document.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <RecalboxConf.h>
#include <recalbox/RecalboxSystem.h>

PatronInfo::PatronInfo(IPatreonNotification* callback)
  : StaticLifeCycleControler<PatronInfo>("PatronInfo")
  , mEvent(this)
  , mToken(RecalboxConf::Instance().GetRecalboxPrivateKey())
  , mCallback(callback)
  , mIsPatron(false)
{
  Thread::Start("PatreonThread");
}

void PatronInfo::Initialize()
{
  if (!mToken.empty())
  {
    // Wait for network interface being available
    // for 300s max
    bool network = false;
    for(int i = 300; --i >= 0; )
    {
      if (RecalboxSystem::hasIpAdress(false) ||
          RecalboxSystem::hasIpAdress(true)) { network = true; break; }
      Thread::Sleep(1000);
    }

    if (network)
    {
      Http http;
      http.SetBearer(mToken);
      std::string url = sRootDomainName;
      std::string body;
      url.append("/userinfo");
      if (http.Execute(url, body))
      {
        bool success = http.GetLastHttpResponseCode() == 200;
        { LOG(LogInfo) << "[Patreon] " << (success ? "Successfully" : "Not") << " authenticated"; }
        if (success)
        {
          rapidjson::Document json;
          json.Parse(body.c_str());
          if (!json.HasParseError())
            if (json.HasMember("full_name"))
            {
              const rapidjson::Value& response = json["full_name"];
              mName = response.GetString();
              mIsPatron = true;
            }
        }
      }
      else { LOG(LogInfo) << "[Patreon] Not a patron"; }
    }
    else { LOG(LogInfo) << "[Patreon] No network"; }
  }
  mEvent.Call(0);
}

void PatronInfo::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  // No need to check event content, there is only one use case
  if (mCallback != nullptr)
    mCallback->PatreonState(mIsPatron, mName);
}
