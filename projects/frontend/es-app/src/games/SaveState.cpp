//
// Created by pre2010-02 on 13/01/23.
//

#include <sys/stat.h>
#include "SaveState.h"
#include "utils/Strings.h"

SaveState::SaveState(const Path& path)
  : mPath(path),
  mThumbnailPath(path.ChangeExtension(path.Extension() + ".png")),
  mIsAuto(false)
{
  struct stat attr{};
  stat(path.ToChars(), &attr);
  mTimer = &attr.st_mtime;
  mModificationDate = ctime(&attr.st_mtime);


  std::string ext = mPath.Extension();
  if (!path.Exists())
    mSlotNumber = -2;

  else if ( ext == ".state")
    mSlotNumber = 0;

  else if ( ext == ".auto")
    mIsAuto = true;

  else
  {
    mSlotNumber = stoi(Strings::Replace(ext, ".state", Strings::Empty));
  }
}
