#pragma once

#include "FolderData.h"

class RootFolderData : public FolderData
{
  public:
    //! Ownership
    enum class Ownership
    {
      None      , //!< This rootfolder has no ownership at all
      FolderOnly, //!< This rootfolder owns all sub folders recursively, but for games
      All       , //!< This rootfolder owns all folder and game structures
    };

  private:
    Ownership mChildOwnership;

  public:
    /*!
     * Constructor
     */
    RootFolderData(Ownership childownership, const Path& startpath, SystemData* system)
      : FolderData(startpath, system),
        mChildOwnership(childownership)
    {
    }

    /*!
     * Since this destructor is called before ~FileData
     * this is the opportuniy to clear child list before they are destroyed
     */
    virtual ~RootFolderData()
    {
      switch(mChildOwnership)
      {
        // Clear child list so that none are dstroyed
        case Ownership::None: ClearChildList(); break;
        // Clear only game, keep the folder structure and let them being destroyed
        case Ownership::FolderOnly: ClearSubChildList(); break;
        // Let everything be detroyed
        case Ownership::All: break;
      }
    }
};


