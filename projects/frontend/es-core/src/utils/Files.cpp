//
// Created by bkg2k on 31/07/2019.
//

#include "Files.h"

std::string Files::LoadFile(const Path& path)
{
  std::string result;

  if (path.Exists())
  {
    FILE* f = fopen(path.ToChars(), "rb");
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

bool Files::SaveFile(const Path& path, const std::string& content)
{
  FILE* f = fopen(path.ToChars(), "wb");
  if (f != nullptr)
  {
    bool ok = (fwrite(content.c_str(), content.size(), 1, f) == 1);
    fclose(f);
    return ok;
  }
  return false;
}
