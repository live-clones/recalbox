//
// Created by bkg2k on 06/01/2020.
//

#include <cstdio>
#include <utils/os/fs/Path.h>
#include <utils/Strings.h>
#include <utils/hash/Md5.h>
#include <utils/Zip.h>

static char buffer[16 << 20]; // 16Mb buffer

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("missing target file argument\n");
    return -1;
  }

  Path path(argv[1]);
  if (!path.Exists())
  {
    printf("file not found\n");
    return -1;
  }

  if (Strings::ToLowerASCII(path.Extension()) == ".zip")
  {
    Zip zip(path);
    printf("%s  %s\n", zip.Md5Composite().c_str(), path.ToChars());
  }
  else
  {
    FILE* f = fopen(path.ToChars(), "rb");
    if (f != nullptr)
    {
      MD5 md5;
      for(int read = 0; (read = fread(buffer, 1, sizeof(buffer), f)) > 0; )
        md5.update(buffer, read);
      fclose(f);
      md5.finalize();
      printf("%s  %s\n", md5.hexdigest().c_str(), path.ToChars());
    }
    else printf("can't open file\n");
  }

  return 0;
}