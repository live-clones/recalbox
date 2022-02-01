//
// Created by bkg2k on 01/05/2020.
//

#include "FakeInternationalizer.h"

std::string Internationalizer::GetText(const char* p, int l)
{
  return std::string(p, l);
}