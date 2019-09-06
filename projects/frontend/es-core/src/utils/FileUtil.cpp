//
// Created by bkg2k on 31/07/2019.
//

#include <fstream>
#include <utils/os/fs/FileSystemUtil.h>
#include "FileUtil.h"

std::string FileUtil::loadTextFile(const std::string& path)
{
  std::string result;

  if (FileSystemUtil::exists(path))
  {
    FILE* f = fopen(path.c_str(), "rb");
    if (f != nullptr)
    {
      fseek(f, 0, SEEK_END);
      long l = ftell(f);
      result.resize(l, 0);
      fseek(f, 0, SEEK_SET);
      if (fread((void*)result.data(), l, 1, f) != 1)
        result.clear();
      fclose(f);
    }
  }

  return result;
}

