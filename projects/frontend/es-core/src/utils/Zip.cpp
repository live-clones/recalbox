//
// Created by bkg2k on 10/12/2019.
//

#include <utils/hash/Md5.h>
#include "Zip.h"

Zip::Zip(const Path& zipfile)
{
  int err;
  mArchive = zip_open(zipfile.ToChars(), ZIP_RDONLY, &err);
}

Zip::~Zip()
{
  if (mArchive != nullptr)
    zip_close(mArchive);
}

int Zip::Count() const
{
  if (mArchive != nullptr)
    return (int)zip_get_num_entries(mArchive, 0);
  return 0;
}

Path Zip::FileName(int index) const
{
  if (mArchive != nullptr)
  {
    const char* name = zip_get_name(mArchive, index, ZIP_FL_ENC_GUESS);
    if (name != nullptr)
      return Path(name);
  }
  return Path::Empty;
}

int Zip::Crc32(int index) const
{
  if (mArchive != nullptr)
  {
    zip_stat_t stats;
    if (zip_stat_index(mArchive, index, 0, &stats) == 0)
      if ((stats.valid & ZIP_STAT_CRC) != 0)
        return stats.crc;
  }
  return 0;
}

std::string Zip::Md5(int index) const
{
  if (mArchive != nullptr)
  {
    MD5 md5;
    zip_file_t* file = zip_fopen_index(mArchive, index, 0);
    if (file != nullptr)
    {
      char buffer[1 << 20]; // 1Mb buffer
      for(int read = 0; (read = (int)zip_fread(file, buffer, sizeof(buffer))) > 0; )
        md5.update(buffer, read);
      zip_fclose(file);
      md5.finalize();
      return md5.hexdigest();
    }
  }

  return std::string();
}

long long Zip::CompressedSize(int index) const
{
  if (mArchive != nullptr)
  {
    zip_stat_t stats;
    if (zip_stat_index(mArchive, index, 0, &stats) == 0)
      if ((stats.valid & ZIP_STAT_COMP_SIZE) != 0)
        return stats.comp_size;
  }
  return 0;
}

long long Zip::UncompressedSize(int index) const
{
  if (mArchive != nullptr)
  {
    zip_stat_t stats;
    if (zip_stat_index(mArchive, index, 0, &stats) == 0)
      if ((stats.valid & ZIP_STAT_SIZE) != 0)
        return stats.size;
  }
  return 0;
}

