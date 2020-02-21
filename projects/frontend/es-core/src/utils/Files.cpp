//
// Created by bkg2k on 31/07/2019.
//

#include "Files.h"
#include "fcntl.h"
#include "unistd.h"

std::string Files::LoadFile(const Path& path)
{
  std::string result;

  if (path.Exists())
  {
    int fd = open(path.ToChars(), O_RDONLY);
    if (fd >= 0)
    {
      long l = lseek(fd, 0, SEEK_END);
      if (l > 0)
      {
        result.resize(l, 0);
        lseek(fd, 0, SEEK_SET);
        if (read(fd, (void*)result.data(), l) != l)
          result.clear();
        close(fd);
      }
    }
  }

  return result;
}

// TODO: convert to open/write/close
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

bool Files::AppendToFile(const Path& path, const void* data, int size)
{
  FILE* f = fopen(path.ToChars(), "ab");
  if (f != nullptr)
  {
    bool ok = (fwrite(data, size, 1, f) == 1);
    fclose(f);
    return ok;
  }
  return false;
}
