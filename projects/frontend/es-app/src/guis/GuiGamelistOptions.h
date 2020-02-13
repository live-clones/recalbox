#include "guis/Gui.h"
#include "components/MenuComponent.h"
#include "components/OptionListComponent.h"
#include "games/FileSorts.h"

class IGameListView;

class GuiGamelistOptions : public Gui
{
public:
	GuiGamelistOptions(Window&window, SystemData* system);
	~GuiGamelistOptions() override;

	void save();
	inline void addSaveFunc(const std::function<void()>& func) { mSaveFuncs.push_back(func); };

	bool ProcessInput(const InputCompactEvent& event) override;
	bool getHelpPrompts(Help& help) override;

private:
  typedef OptionListComponent<unsigned int> LetterList;
  typedef OptionListComponent<int> SortList;
  typedef OptionListComponent<int> RegionList;

  void openMetaDataEd();
	void jumpToLetter();
	
	MenuComponent mMenu;

	std::vector< std::function<void()> > mSaveFuncs;
	std::shared_ptr<LetterList> mJumpToLetterList;
	std::shared_ptr<SortList> mListSort;
  std::shared_ptr<RegionList> mListRegion;

	bool mFavoriteState;
	bool mHiddenState;
	bool mReloading;

	SystemData* mSystem;
	IGameListView* getGamelist();
};
