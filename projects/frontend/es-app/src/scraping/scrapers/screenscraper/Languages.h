//
// Created by gugue_u on 20/02/2021.
//

#pragma once

#include <vector>

enum class Languages
{
    EN,
    ES,
    PT,
    FR,
    DE,
    IT,
    NL,
    JA,
    ZH,
    KO,
    RU,
    DA,
    FI,
    SV,
    HU,
    NO,
    PL,
    CZ,
    SK,
    TR,
    Unknown,
};

class LanguagesTools
{
  public:

    static const std::string& LanguagesFullName(Languages language);

    typedef std::vector<Languages> List;
    static const List& AvailableLanguages();


    static Languages DeserializeLanguage(const std::string& language);
    static const std::string& SerializeLanguage(Languages language);


    static Languages LanguageFromString(const std::string& languages);

    static const std::string& LanguageFromEnum(Languages languages);

    static Languages GetScrapingLanguage();
};