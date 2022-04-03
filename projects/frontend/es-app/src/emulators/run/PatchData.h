//
// Created by gugue_u on 01/04/2022.
//

#pragma once


class PatchData
{
  public:
    PatchData()
      : mDisabledSoftpatching(false),
        mPatchConfigured(false)
    {};

    Path PatchFile() const { return mPatchFile; }
    bool DisabledSofpatching() const { return mDisabledSoftpatching; }

    void SetPatchFile(const Path& patchFile)
    {
      mPatchFile = patchFile;
      mPatchConfigured = true;
    }

    void SetDisabledSoftPatching(bool softpatch)
    { mDisabledSoftpatching = softpatch;
      mPatchConfigured = true;
    }

    bool IsConfigured()
    {
      return mPatchConfigured;
    }

  private:
    Path mPatchFile;
    bool mDisabledSoftpatching;
    bool mPatchConfigured;
};
