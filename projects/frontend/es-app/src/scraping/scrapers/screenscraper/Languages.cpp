//
// Created by gugue_u on 20/02/2021.
//

#include <string>
#include <utils/storage/HashMap.h>
#include "Languages.h"
#include "RecalboxConf.h"

const std::string& LanguagesTools::LanguagesFullName(Languages language)
{
  static HashMap<Languages, const std::string> sLanguagesToFullName({
    {Languages::EN, "English"},
    {Languages::FR, "Français"},
    {Languages::ES, "Español"},
    {Languages::PT, "Português"},
    {Languages::DE, "Deutsch"},
    {Languages::IT, "Italiano"},
    {Languages::NL, "Nederlands"},
    {Languages::JA, "日本語"},
    {Languages::RU, "Русский"},
    {Languages::DA, "Dansk"},
    {Languages::FI, "Suomi"},
    {Languages::HU, "Magyar"},
    {Languages::NO, "Norsk"},
    {Languages::PL, "Polski"},
    {Languages::CZ, "Čeština"},
    {Languages::SK, "Slovenčina"},
    {Languages::TR, "Türkçe"},
    {Languages::ZH, "简体中文"},
    {Languages::KO, "한국어"},
    {Languages::SV, "Svenska"},
  });

  const std::string* found = sLanguagesToFullName.try_get(language);
  if (found != nullptr)
    return *found;

  static std::string sUnknown("Unknown Language");
  return sUnknown;
}

const LanguagesTools::List& LanguagesTools::AvailableLanguages()
{
  static List sLanguages
    ({
       Languages::CZ,
       Languages::DA,
       Languages::DE,
       Languages::ES,
       Languages::EN,
       Languages::FR,
       Languages::IT,
       Languages::JA,
       Languages::KO,
       Languages::NL,
       Languages::NO,
       Languages::HU,
       Languages::PL,
       Languages::PT,
       Languages::RU,
       Languages::SK,
       Languages::FI,
       Languages::SV,
       Languages::TR,
       Languages::ZH,
     });
  return sLanguages;
}

Languages LanguagesTools::DeserializeLanguage(const std::string& language)
{
  static HashMap<std::string, Languages> sLanguageNameToLanguage
  ({
      {"en", Languages::EN},
      {"es", Languages::ES},
      {"pt", Languages::PT},
      {"fr", Languages::FR},
      {"de", Languages::DE},
      {"it", Languages::IT},
      {"nl", Languages::NL},
      {"ja", Languages::JA},
      {"zh", Languages::ZH},
      {"ko", Languages::KO},
      {"ru", Languages::RU},
      {"da", Languages::DA},
      {"fi", Languages::FI},
      {"sv", Languages::SV},
      {"hu", Languages::HU},
      {"no", Languages::NO},
      {"pl", Languages::PL},
      {"cz", Languages::CZ},
      {"sk", Languages::SK},
      {"tr", Languages::TR},
    });

  Languages* found = sLanguageNameToLanguage.try_get(language);
  if (found != nullptr)
    return *found;

  return Languages::Unknown;
}

const std::string& LanguagesTools::SerializeLanguage(Languages language)
{
  static HashMap<Languages, std::string> sLanguageToLanguageName
    ({
       {Languages::EN, "en"},
       {Languages::ES, "es"},
       {Languages::PT, "pt"},
       {Languages::FR, "fr"},
       {Languages::DE, "de"},
       {Languages::IT, "it"},
       {Languages::NL, "nl"},
       {Languages::JA, "ja"},
       {Languages::ZH, "zh"},
       {Languages::KO, "ko"},
       {Languages::RU, "ru"},
       {Languages::DA, "da"},
       {Languages::FI, "fi"},
       {Languages::SV, "sv"},
       {Languages::HU, "hu"},
       {Languages::NO, "no"},
       {Languages::PL, "pl"},
       {Languages::CZ, "cz"},
       {Languages::SK, "sk"},
       {Languages::TR, "tr"},
     });

  std::string* found = sLanguageToLanguageName.try_get(language);
  if (found != nullptr)
    return *found;

  static std::string sWorld("Unknown");
  return sWorld;
}

Languages LanguagesTools::LanguageFromString(const std::string& language)
{
  return DeserializeLanguage(language);
}

const std::string& LanguagesTools::LanguageFromEnum(Languages language)
{
  return SerializeLanguage(language);
}

Languages LanguagesTools::GetScrapingLanguage()
{
  Languages confLanguage = RecalboxConf::Instance().GetScreenScraperLanguage();
  if(Languages::Unknown != confLanguage)
    return confLanguage;

  const std::string languagesStd = RecalboxConf::Instance().GetSystemLanguage();
  Languages systemLanguage =  LanguagesTools::DeserializeLanguage(languagesStd.substr(0, 2));
  if(Languages::Unknown != systemLanguage)
  {
    return systemLanguage;
  }

  return Languages::EN;
}