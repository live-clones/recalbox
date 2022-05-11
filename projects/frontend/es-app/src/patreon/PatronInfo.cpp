//
// Created by digitalLumberjack on 08/05/2022.
//

#include "PatronInfo.h"
#include "utils/network/Http.h"
#include "utils/Log.h"
#include <rapidjson/document.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <RecalboxConf.h>

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
            const rapidjson::Value &response = json["full_name"];
            mName = response.GetString();
            mIsPatron = true;
          }
      }
    } else { LOG(LogInfo) << "[Patreon] Not a patron"; }
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
