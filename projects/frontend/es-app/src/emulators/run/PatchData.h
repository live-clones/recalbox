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

    bool DisabledSofpatching() const { return mDisabledSoftpatching; }

    void SetDisabledSoftPatching(bool softpatch)
    { mDisabledSoftpatching = softpatch;
      mPatchConfigured = true;
    }

    bool IsConfigured()
    {
      return mPatchConfigured;
    }

  private:
    bool mDisabledSoftpatching;
    bool mPatchConfigured;
};
