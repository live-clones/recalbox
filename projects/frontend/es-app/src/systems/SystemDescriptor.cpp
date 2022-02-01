//
// Created by bkg2k on 19/06/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <systems/SystemDescriptor.h>

std::string SystemDescriptor::mDefaultCommand;  //!< Default command
std::string SystemDescriptor::IconPrefix() const
{
  return Strings::unicode2Chars((unsigned int)mIcon).append(1, ' ');
}
