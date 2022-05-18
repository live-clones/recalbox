//
// Created by bkg2k on 31/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "DnsClient.h"
#include <utils/Log.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <cstring>
#include <arpa/inet.h>

std::string DnsClient::GetTxtRecord(const std::string& domain)
{
  Entry* entry = mCache.try_get(domain);
  if (entry != nullptr)
    if (entry->TxtRecord().IsValid())
      return entry->TxtRecord().Data();

  // Query TXT
  unsigned char buffer[4096];
  int l = res_query(domain.c_str(), C_IN, ns_t_txt, buffer, sizeof (buffer));
  if (l >= 0)
  {
    ns_msg msg;
    ns_initparse(buffer, l, &msg);
    ns_rr rr;
    if (ns_parserr(&msg, ns_s_an, 0, &rr) == 0)
    {
      std::string result = std::string((char*) (ns_rr_rdata(rr) + 1), (int) ns_rr_rdata(rr)[0]);
      { LOG(LogDebug) << "[Update] Domain: " << result; }

      if (entry == nullptr) entry = &mCache[domain];
      entry->SetTxtRecord(result);

      return result;
    }
    else { LOG(LogError) << "[DnsClient] Error parsing DNS response! (" << domain << ')'; }
  }
  else { LOG(LogError) << "[DnsClient] Error getting DNS Txt record from " << domain; }

  return std::string();
}

std::string DnsClient::GetARecord(const std::string& domain)
{
  Entry* entry = mCache.try_get(domain);
  if (entry != nullptr)
    if (entry->ARecord().IsValid())
      return entry->ARecord().Data();

  // Query TXT
  unsigned char buffer[4096];
  int l = res_query(domain.c_str(), C_IN, ns_t_a, buffer, sizeof (buffer));
  if (l >= 0)
  {
    ns_msg msg;
    ns_initparse(buffer, l, &msg);
    ns_rr rr;
    std::string result;
    for(int count = ns_msg_count(msg, ns_s_an); --count >= 0; )
    {
      if (ns_parserr(&msg, ns_s_an, count, &rr) == 0)
      {
        struct in_addr in {};
        memcpy(&in.s_addr, ns_rr_rdata(rr), sizeof(in.s_addr));
        result.append(inet_ntoa(in));
        if (count != 0) result.append(1, '|');
        { LOG(LogDebug) << "[Update] " << ns_rr_name(rr) << " in " << inet_ntoa(in); }
      }
      else { LOG(LogError) << "[DnsClient] Error parsing DNS response! (" << domain << ')'; }
    }
    { LOG(LogDebug) << "[Update] Domain: " << result; }

    if (entry == nullptr) entry = &mCache[domain];
    entry->SetARecord(result);

    return result;
  }
  else { LOG(LogError) << "[DnsClient] Error getting DNS Txt record from " << domain; }

  return std::string();
}
