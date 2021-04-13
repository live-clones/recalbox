//
// Created by bkg2k on 28/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>
#include <views/gamelist/IGameListView.h>

// Forward declaration
class SystemManager;
class SystemData;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuGamelistGameOptions : public GuiMenuBase
                                 , private IOptionListComponent<std::string>
                                 , private IOptionListComponent<GameGenres>
                                 , private ISwitchComponent
                                 , private IEditableComponent
                                 , private IRatingComponent
                                 , private IGuiMenuBase
                                 , private GuiScraperSingleGameRun::IScrappingComplete
{
  public:
    GuiMenuGamelistGameOptions(WindowManager& window, IGameListView& view, SystemManager& systemManager, SystemData& system, FileData& game);

  private:
    enum class Components
    {
      Emulator,
      Ratio,
      Favorite,
      Hidden,
      Adult,
      Name,
      Description,
      Rating,
      Genre,
      Scrape,
    };

    //! View reference
    IGameListView& mView;
    //! System Manager reference
    SystemManager& mSystemManager;
    //! System reference
    SystemData& mSystem;
    //! Game reference
    FileData& mGame;

    //! Default emulator
    std::string mDefaultEmulator;
    //! Default core
    std::string mDefaultCore;

    //! Emulator/Core
    std::shared_ptr<OptionListComponent<std::string>> mEmulator;
    //! Ratio
    std::shared_ptr<OptionListComponent<std::string>> mRatio;
    //! Name
    std::shared_ptr<EditableComponent> mName;
    //! Rating
    std::shared_ptr<RatingComponent> mRating;
    //! Genre
    std::shared_ptr<OptionListComponent<GameGenres>> mGenre;
    //! Description
    std::shared_ptr<EditableComponent> mDescription;
    //! Favorite
    std::shared_ptr<SwitchComponent> mFavorite;
    //! Hidden
    std::shared_ptr<SwitchComponent> mHidden;
    //! Adult
    std::shared_ptr<SwitchComponent> mAdult;

    //! Emulator/Core list
    std::shared_ptr<IOptionListComponent<std::string>> mEmulators;

    //! Get emulator list
    std::vector<ListEntry<std::string>> GetEmulatorEntries();
    //! Get ratio list
    std::vector<ListEntry<std::string>> GetRatioEntries();
    //! Get rating list
    std::vector<ListEntry<float>> GetRatingEntries();
    //! Get genre list
    std::vector<ListEntry<GameGenres>> GetGenreEntries();

    /*
     * IOptionListComponent<std::string> implementation
     */

    void OptionListComponentChanged(int id, int index, const std::string& value) override;

    /*
     * IOptionListComponent<GameGenres> implementation
     */

    void OptionListComponentChanged(int id, int index, const GameGenres& value) override;

    /*
     * ISwitchComponent implementation
     */

    void EditableComponentTextChanged(int id, const std::string& text) override;

    /*
     * IEditableComponent implementation
     */

    void SwitchComponentChanged(int id, bool status) override;

    /*
     * IRatingComponent implementation
     */

    void RatingChanged(int id, float value) override;

    /*
     * IGuiMenuBase implementation
     */

    void SubMenuSelected(int id) override;

    /*
     * GuiScraperSingleGameRun::IScrappingComplete implementation
     */

    void ScrappingComplete(FileData& game) override;
};



