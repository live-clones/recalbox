//
// Created by bkg2k on 13/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <RecalboxConf.h>
#include <utils/locale/LocaleHelper.h>
#include <utils/locale/Internationalizer.h>
#include "WizardLanguage.h"
#include <help/Help.h>

void WizardLanguage::OnHelpRequired(int page, Help& help)
{
  (void)page;
  help.Set(HelpType::UpDown, _("LANGUAGE"));
}

void WizardLanguage::ChangeLanguage(bool increment)
{
  static constexpr int sLanguageCount = 6;
  static const char* sAvailableLanguages[sLanguageCount] =
    {
      "en_US",
      "fr_FR",
      "pt_BR",
      "de_DE",
      "es_ES",
      "it_IT"
    };

  // Seek current
  int current = 0;
  std::string currentLanguage = RecalboxConf::Instance().GetSystemLanguage();
  for(int i = sLanguageCount; --i >= 0; )
    if (currentLanguage == sAvailableLanguages[i])
    {
      current = i;
      break;
    }

  // Shift language
  current += increment ? 1 : -1;
  if (current < 0) current = sLanguageCount - 1;
  else if (current >= sLanguageCount) current = 0;

  // Change language
  RecalboxConf::Instance().SetSystemLanguage(sAvailableLanguages[current]);
  // Set locale
  if (!Internationalizer::InitializeLocale(sAvailableLanguages[current], { Path("/usr/share/locale") }, "emulationstation2"))
  { LOG(LogWarning) << "[Locale] No locale found. Default text used."; }

  // Refresh all
  UpdatePage();
  updateHelpPrompts();
}
