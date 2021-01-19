//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <vector>
#include <guis/Gui.h>
#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>

// Forward declaration
class SwitchComponent;
template<typename T> class OptionListComponent;

class GuiMenuBase : public Gui
{
  public:
    //! Constructor
    explicit GuiMenuBase(WindowManager& window, const std::string& title);

    /*!
     * @brief Proces input
     * @param event Inpuit event
     * @return True if the input has been processed
     */
    bool ProcessInput(const InputCompactEvent& event) override;

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update() to ensure animation and childrens are updated properly
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Fill in help
     * @param help Help
     * @return true of the help system has been files
     */
    bool getHelpPrompts(Help& help) override;

  protected:
    //! Menu
    MenuComponent mMenu;
    //! Menu theme
    const MenuTheme& mTheme;
    //! Menu initialized flag
    bool mMenuInitialized;

    //! Reboot
    static void Reboot();

    //! Notify the user Recalbox is about to reboot
    void RequestReboot();

    /*!
     * @brief Add Submenu
     * @param label Label
     * @param func Method to call back when selected
     * @param help Help string
     */
    void AddSubMenu(const std::string& label, const std::function<void()>& func, const std::string& help);

    /*!
     * @brief Add Submenu
     * @param label Label
     * @param func Method to call back when selected
     */
    void AddSubMenu(const std::string& label, const std::function<void()>& func);

    /*!
     * @brief Add Submenu with Icon
     * @param label Label
     * @param icon Icon image path
     * @param func Method to call back when selected
     * @param help Help string
     */
    void AddSubMenu(const std::string& label, const Path& icon, const std::function<void()>& func, const std::string& help);

    /*!
     * @brief Add Submenu with Icon
     * @param label Label
     * @param icon Icon image path
     * @param func Method to call back when selected
     */
    void AddSubMenu(const std::string& label, const Path& icon, const std::function<void()>& func);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @param help Help text
     * @return Switch component
     */
    std::shared_ptr<SwitchComponent> AddSwitch(const std::string& text, bool value, const std::function<void(bool)>& callback, const std::string& help);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @return Switch component
     */
    std::shared_ptr<SwitchComponent> AddSwitch(const std::string& text, bool value, const std::function<void(bool)>& callback);

    //! Normalized entry structure for List components
    template<typename T> struct ListEntry
    {
      std::string mText; //!< Displayed text
      T mValue;          //!< Linked Values
      bool mSelected;    //!< Whether the entry is selected or not

      //! Constructor
      ListEntry(const std::string& t, const T& v, bool s)
        : mText(t)
        , mValue(v)
        , mSelected(s)
      {}
    };

    /*!
     * @brief Add s choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @param entries Entries to add to the list
     * @param help Help text
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddList(const std::string& text, const std::function<void(const T&)>& callback, const std::vector<ListEntry<T>>& entries, const std::string& help)
    {
      auto result = std::make_shared<OptionListComponent<T>>(mWindow, text, false);
      result->setSelectedChangedCallback(callback);
      for(const ListEntry<T>& entry : entries)
        result->add(entry.mText, entry.mValue, entry.mSelected);
      mMenu.addWithLabel(result, text, help);
      return result;
    }

    /*!
     * @brief Add s choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @param entries Entries to add to the list
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddList(const std::string& text, const std::function<void(const T&)>& callback, const std::vector<ListEntry<T>>& entries)
    {
      return AddList(text, callback, entries, Strings::Empty);
    }
};
