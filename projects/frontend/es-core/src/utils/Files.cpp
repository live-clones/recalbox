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
        l = read(fd, (void*)result.data(), l);
        if (l < 0) l = 0; // Return empty string on error
        result.resize(l);
      }
      else if (l < 0)
      {
        char buffer[4096];
        for(int r = 0; (r = read(fd, buffer, sizeof(buffer))) > 0; )
          result.append(buffer, r);
      }
      close(fd);
    }
  }

  return result;
}

std::string Files::LoadFile(const Path& path, long long from, long long size)
{
  std::string result;

  if (path.Exists())
  {
    int fd = open(path.ToChars(), O_RDONLY);
    if (fd >= 0)
    {
      long l = lseek(fd, from, SEEK_END);
      if (l > 0)
      {
        result.resize(size, 0);
        lseek(fd, 0, SEEK_SET);
        l = read(fd, (void*)result.data(), size);
        if (l < 0) l = 0; // Return empty string on error
        result.resize(l);
      }
      close(fd);
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

bool Files::CopyFile(const Path& from, const Path& to)
{
  // Source exists?
  if (!from.Exists()) return false;
  // Make destination
  if (!to.Directory().CreatePath()) return false;

  bool result = false;
  int fdr = open(from.ToChars(), O_RDONLY);
  int fdw = open(to.ToChars(), O_CREAT | O_WRONLY);
  if (fdr >= 0 && fdw >= 0)
    for(;;)
    {
      unsigned char buffer[sBufferSize]; // 1Mb buffer
      long l = lseek(fdr, 0, SEEK_END);
      lseek(fdr, 0, SEEK_SET);
      for(long i = l >> sBufferSizeBits; --i >= 0;)
      {
        if (read(fdr, buffer, sBufferSize) != sBufferSize) break; // Error
        if (write(fdw, buffer, sBufferSize) != sBufferSize) break; // Error
      }
      if ((l &= sBufferSizeMask) != 0)
      {
        if (read(fdr, buffer, l) != l) break; // Error
        if (write(fdw, buffer, l) != l) break; // Error
      }
      result = true; // Success
      break;
    }
  if (fdr >= 0) close(fdr);
  if (fdw >= 0) close(fdw);

  return result;
}

bool Files::CopyFolder(const Path& from, const Path& to, bool recurse)
{
  // Source exists?
  if (!from.Exists()) return false;
  // Make destination
  if (!to.Directory().CreatePath()) return false;

  // Get everything
  for(const Path& path : from.GetDirectoryContent())
  {
    if (path.IsFile()) if (!CopyFile(path, to / path.Filename())) return false;
    if (path.IsDirectory())
    {
      if (recurse) { if (!CopyFolder(path, to / path.Filename(), true)) return false; }
      else { if (!(to / path.Filename()).CreatePath()) return false; }
    }
  }
  return true;
}


