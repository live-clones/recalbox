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
  , mToken(Strings::Trim(RecalboxConf::Instance().GetRecalboxPrivateKey(), " \t"))
  , mCallback(callback)
  , mResult(PatronAuthenticationResult::Unknown)
  , mLevel(0)
  , mDone(false)
{
  Thread::Start("PatreonThread");
}

void PatronInfo::Initialize()
{
  if (!mToken.empty())
  {
    unsigned int start = SDL_GetTicks();
    while(SDL_GetTicks() - start < sNetworkTimeout)
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
              break;
            }
            else
            {
              LOG(LogError) << "[Patreon] API error, missing fields";
              mResult = PatronAuthenticationResult::ApiError;
              break;
            }
          }
          else
          {
            LOG(LogError) << "[Patreon] Json parsing error";
            mResult = PatronAuthenticationResult::ApiError;
            break;
          }
        }
        else
        {
          if (http.GetLastHttpResponseCode() == 401)
          {
            LOG(LogError) << "[Patreon] Invalid key!";
            mResult = PatronAuthenticationResult::Invalid;
            break;
          }
          else
          {
            LOG(LogError) << "[Patreon] Http error: " << http.GetLastHttpResponseCode();
            mResult = PatronAuthenticationResult::HttpError;
            Thread::Sleep(10000); // Wait 10s & retry
          }
        }
      }
      else
      {
        LOG(LogError) << "[Patreon] Unknown Http error";
        mResult = PatronAuthenticationResult::HttpError;
        Thread::Sleep(1000); // Wait 1s & retry
      }
    }

    if (SDL_GetTicks() - start >= sNetworkTimeout)
    {
      LOG(LogError) << "[Patreon] No network";
      mResult = PatronAuthenticationResult::NetworkError;
    }
  }

  if (mResult == PatronAuthenticationResult::Unknown)
    mResult = PatronAuthenticationResult::NoPatron;

  mEvent.Call((int)mResult);
}

void PatronInfo::ReceiveSyncCallback(const SDL_Event& event)
{
  (void)event;
  // No need to check event content, there is only one use case
  if (mCallback != nullptr)
    mCallback->PatreonState(mResult, mLevel, mName);
}

void PatronInfo::WaitForAuthentication(Thread& caller)
{
  while(!mDone && caller.IsRunning())
    Thread::Sleep(1000);
}
