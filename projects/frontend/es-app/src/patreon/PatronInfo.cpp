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
  , mResult(PatronAuthenticationResult::NoPatron)
  , mLevel(0)
{
  Thread::Start("PatreonThread");
}

void PatronInfo::Initialize()
{
  mResult = PatronAuthenticationResult::NoPatron;

  if (!mToken.empty())
  {
    // Wait for network interface being available
    // for 300s max
    bool network = false;
    for(int i = 300; --i >= 0 && IsRunning(); )
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
          {
            if (json.HasMember("full_name") && json.HasMember("patron_status"))
            {
              const rapidjson::Value& response = json["full_name"];
              mName = response.GetString();
              bool active = std::string(json["patron_status"].GetString()) == "active_patron";
              mResult = active ? PatronAuthenticationResult::Patron : PatronAuthenticationResult::FormerPatron;
              mLevel = json["tier_status"].GetInt();
            }
            else
            {
              LOG(LogInfo) << "[Patreon] API error, missing fields";
              mResult = PatronAuthenticationResult::ApiError;
              //mResult = PatronAuthenticationResult::Patron;
              //mName = "PATRON";
              //mLevel = 3;
            }
          }
          else
          {
            LOG(LogInfo) << "[Patreon] Json parsing error";
            mResult = PatronAuthenticationResult::ApiError;
          }
        }
        else
        {
          if (http.GetLastHttpResponseCode() == 401)
          {
            LOG(LogInfo) << "[Patreon] Invalid key!";
            mResult = PatronAuthenticationResult::Invalid;
          }
          else
          {
            LOG(LogInfo) << "[Patreon] Http error: " << http.GetLastHttpResponseCode();
            mResult = PatronAuthenticationResult::HttpError;
          }
        }
      }
      else
      {
        LOG(LogInfo) << "[Patreon] Unknown Http error";
        mResult = PatronAuthenticationResult::HttpError;
      }
    }
    else
    {
      LOG(LogInfo) << "[Patreon] No network";
      mResult = PatronAuthenticationResult::NetworkError;
    }
  }

  mEvent.Call((int)mResult);
}

void PatronInfo::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  // No need to check event content, there is only one use case
  if (mCallback != nullptr)
    mCallback->PatreonState(mResult, mLevel, mName);
}
