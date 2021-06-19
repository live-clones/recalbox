#pragma once

#include "FolderData.h"

class RootFolderData : public FolderData
{
  public:
    //! Ownership
    enum class Ownership : char
    {
      None      , //!< This rootfolder has no ownership at all
      FolderOnly, //!< This rootfolder owns all sub folders recursively, but for games
      All       , //!< This rootfolder owns all folder and game structures
    };

    enum class Types : char
    {
      None    , //!< Nothing particular
      ReadOnly, //!< Read only tree
      Virtual , //!< Virtual tree
    };

    /*!
     * Constructor
     */
    RootFolderData(RootFolderData& topAncestor, Ownership childownership, Types type, const Path& startpath, SystemData& system)
      : FolderData(topAncestor, startpath)
      , mSystem(system)
      , mChildOwnership(childownership)
      , mType(type)
    {
    }

    /*!
     * Since this destructor is called before ~FileData
     * this is the opportuniy to clear child list before they are destroyed
     */
    virtual ~RootFolderData()
    {
      // Destroy any subroot first
      for(RootFolderData* root : SubRoots())
      {
        RemoveChild(root);
        delete root;
      }

      // Then remaining items
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

    //! System
    SystemData& RootSystem() const { return mSystem; }

    //! Get type
    Types RootType() const { return mType; }

    //! Normal folder?
    bool Normal() const { return (mType == Types::None); }

    //! Is read only?
    bool ReadOnly() const { return (mType == Types::ReadOnly); }

    //! Virtual folder?
    bool Virtual() const { return (mType == Types::Virtual); }

    //! Has sub root?
    bool HasSubRoots() const
    {
      for(FileData* child : mChildren)
        if (child->IsRoot())
          return true;
      return false;
    }

    //! Get sub-root folders
    std::vector<RootFolderData*> SubRoots() const
    {
      std::vector<RootFolderData*> roots;
      for(FileData* child : mChildren)
        if (child->IsRoot())
          roots.push_back((RootFolderData*)child);
      return roots;
    }

    /*!
     * @brief Add Sub-root
     * @param subroot Sub-root to add
     */
    void AddSubRoot(RootFolderData* subroot) { AddChild(subroot, true); }

  private:
    //! Parent system
    SystemData& mSystem;
    //! Type of child ownership
    Ownership mChildOwnership;
    //! This folder and all its subtree is readonly
    Types mType;

    // Hide addChild
    using FolderData::AddChild;
};
