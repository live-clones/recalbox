//
// Created by bkg2k on 31/10/2020.
//
#pragma once

#include <vector>
#include <guis/Gui.h>
#include <components/MenuComponent.h>
#include <components/OptionListComponent.h>
#include <components/IEditableComponent.h>
#include <components/ISwitchComponent.h>
#include <components/ISliderComponent.h>
#include <components/IRatingComponent.h>
#include "IGuiMenuBase.h"

// Forward declaration
class SwitchComponent;
class SliderComponent;
class RatingComponent;
class EditableComponent;
template<typename T> class OptionListComponent;

class GuiMenuBase : public Gui, private IComponentListRow
{
  public:
    //! Constructor
    explicit GuiMenuBase(WindowManager& window, const std::string& title, IGuiMenuBase* interface);

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

    void SetFooter(const std::string& footer)
    {
      mFooter = footer;
      mMenuInitialized = false;
    }

  protected:
    //! Menu
    MenuComponent mMenu;
    //! Menu theme
    const MenuTheme& mTheme;

    //! Notify the user Recalbox is about to reboot
    void RequestReboot();

    //! Request a relaunch
    void RequestRelaunch();

    /*!
     * @brief Add Submenu
     * @param label Label
     * @param func Method to call back when selected
     * @param help Help string
     */
    std::shared_ptr<TextComponent> AddSubMenu(const std::string& label, int id, const std::string& help);

    /*!
     * @brief Add Submenu
     * @param label Label
     * @param func Method to call back when selected
     */
    std::shared_ptr<TextComponent> AddSubMenu(const std::string& label, int id);

    /*!
     * @brief Add Submenu with Icon
     * @param label Label
     * @param icon Icon image path
     * @param func Method to call back when selected
     * @param help Help string
     */
    std::shared_ptr<TextComponent> AddSubMenu(const std::string& label, const Path& icon, int id, const std::string& help);

    /*!
     * @brief Add Submenu with Icon
     * @param label Label
     * @param icon Icon image path
     * @param func Method to call back when selected
     */
    std::shared_ptr<TextComponent> AddSubMenu(const std::string& label, const Path& icon, int id);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @param help Help text
     * @return Switch component
     */
    std::shared_ptr<SwitchComponent> AddSwitch(const std::string& text, bool value, int id, ISwitchComponent* interface, const std::string& help);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @return Switch component
     */
    std::shared_ptr<SwitchComponent> AddSwitch(const std::string& text, bool value, int id, ISwitchComponent* interface);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @param help Help text
     * @return Switch component
     */
    std::shared_ptr<RatingComponent> AddRating(const std::string& text, float value, int id, IRatingComponent* interface, const std::string& help);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @return Switch component
     */
    std::shared_ptr<RatingComponent> AddRating(const std::string& text, float value, int id, IRatingComponent* interface);

    /*!
     * @brief Add a switch menu entry
     * @param icon
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @param help Help text
     * @return Switch component
     */
    std::shared_ptr<SwitchComponent> AddSwitch(const Path& icon, const std::string& text, bool value, int id, ISwitchComponent* interface, const std::string& help);

    /*!
     * @brief Add a switch menu entry
     * @param text Menu text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @return Switch component
     */
    std::shared_ptr<SwitchComponent> AddSwitch(const Path& icon, const std::string& text, bool value, int id, ISwitchComponent* interface);

    /*!
     * @brief Add non editable text
     * @param text Caption text
     * @param value Text value
     * @param help Help text
     * @return TextComponent
     */
    std::shared_ptr<TextComponent> AddText(const std::string& text, const std::string& value, const std::string& help);

    /*!
     * @brief Add non editable text
     * @param text Caption text
     * @param value Text value
     * @return TextComponent
     */
    std::shared_ptr<TextComponent> AddText(const std::string& text, const std::string& value);

    /*!
     * @brief Add non editable text
     * @param text Caption text
     * @param value Text value
     * @param color Text color
     * @param help Help text
     * @return TextComponent
     */
    std::shared_ptr<TextComponent> AddText(const std::string& text, const std::string& value, unsigned int color, const std::string& help);

    /*!
     * @brief Add non editable text
     * @param text Caption text
     * @param value Text value
     * @param color Text color
     * @return TextComponent
     */
    std::shared_ptr<TextComponent> AddText(const std::string& text, const std::string& value, unsigned int color);

    /*!
     * @brief Add editable text
     * @param text Caption text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @param help Help text
     * @return TextComponent
     */
    std::shared_ptr<EditableComponent> AddEditable(const std::string& edittitle, const std::string& text, const std::string& value, int id, IEditableComponent* interface, const std::string& help, bool masked);

    /*!
     * @brief Add editable text
     * @param text Caption text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @return TextComponent
     */
    std::shared_ptr<EditableComponent> AddEditable(const std::string& edittitle, const std::string& text, const std::string& value, int id, IEditableComponent* interface, bool masked);

    /*!
     * @brief Add editable text
     * @param text Caption text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @param help Help text
     * @return TextComponent
     */
    std::shared_ptr<EditableComponent> AddEditable(const std::string& text, const std::string& value, int id, IEditableComponent* interface, const std::string& help, bool masked);

    /*!
     * @brief Add editable text
     * @param text Caption text
     * @param value Initial value
     * @param callback Callback when entry changes
     * @return TextComponent
     */
    std::shared_ptr<EditableComponent> AddEditable(const std::string& text, const std::string& value, int id, IEditableComponent* interface, bool masked);

    /*!
     * @brief Add a Slider menu entry
     * @param text Menu text
     * @param min Minimum value
     * @param max Maximum value
     * @param value Initial value
     * @param id Identifier
     * @param interface Callback interface
     * @param help Help text
     * @return Slider component
     */
    std::shared_ptr<SliderComponent> AddSlider(const std::string& text, float min, float max, float inc, float value, const std::string& suffix, int id, ISliderComponent* interface, const std::string& help);

    /*!
     * @brief Add a Slider menu entry
     * @param text Menu text
     * @param min Minimum value
     * @param max Maximum value
     * @param value Initial value
     * @param id Identifier
     * @param interface Callback interface
     * @return Slider component
     */
    std::shared_ptr<SliderComponent> AddSlider(const std::string& text, float min, float max, float inc, float value, const std::string& suffix, int id, ISliderComponent* interface);

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
     * @brief Add a choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param id Identifier
     * @param interface callback interface
     * @param entries Entries to add to the list
     * @param help Help text
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddList(const std::string& text, int id, IOptionListComponent<T>* interface, const std::vector<ListEntry<T>>& entries, const std::string& help)
    {
      auto result = std::make_shared<OptionListComponent<T>>(mWindow, text);
      if (!entries.empty())
      {
        for (const ListEntry<T>& entry : entries)
          result->add(entry.mText, entry.mValue, entry.mSelected);
        mMenu.addWithLabel(result, text, help);
        result->SetInterface(id, interface);
      }
      return result;
    }

    /*!
     * @brief Add a choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @param entries Entries to add to the list
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddList(const std::string& text, int id, IOptionListComponent<T>* interface, const std::vector<ListEntry<T>>& entries)
    {
      return AddList(text, id, interface, entries, Strings::Empty);
    }

    /*!
     * @brief Add an empty choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @param help Help text
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddList(const std::string& text, int id, IOptionListComponent<T>* interface, const std::string& help)
    {
      auto result = std::make_shared<OptionListComponent<T>>(mWindow, text);
      mMenu.addWithLabel(result, text, help);
      result->SetInterface(id, interface);
      return result;
    }

    /*!
     * @brief Add an empty choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddList(const std::string& text, int id, IOptionListComponent<T>* interface)
    {
      return AddList(text, id, interface, Strings::Empty);
    }

    /*!
     * @brief Add a multi-choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param id Identifier
     * @param interface callback interface
     * @param entries Entries to add to the list
     * @param help Help text
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddMultiList(const std::string& text, int id, IOptionListMultiComponent<T>* interface, const std::vector<ListEntry<T>>& entries, const std::string& help)
    {
      auto result = std::make_shared<OptionListComponent<T>>(mWindow, text, true);
      if (!entries.empty())
      {
        for (const ListEntry<T>& entry : entries)
          result->add(entry.mText, entry.mValue, entry.mSelected);
        mMenu.addWithLabel(result, text, help);
        result->SetMultiInterface(id, interface);
      }
      return result;
    }

    /*!
     * @brief Add a multi-choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @param entries Entries to add to the list
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddMultiList(const std::string& text, int id, IOptionListMultiComponent<T>* interface, const std::vector<ListEntry<T>>& entries)
    {
      return AddMultiList(text, id, interface, entries, Strings::Empty);
    }

    /*!
     * @brief Add an empty multi-choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @param help Help text
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddMultiList(const std::string& text, int id, IOptionListMultiComponent<T>* interface, const std::string& help)
    {
      auto result = std::make_shared<OptionListComponent<T>>(mWindow, text, true);
      mMenu.addWithLabel(result, text, help);
      result->SetMultiInterface(id, interface);
      return result;
    }

    /*!
     * @brief Add an empty multi-choice list
     * @tparam T Select object type
     * @param text Menu text (and selection window title)
     * @param callback Callback when a new value is selected
     * @return OptionListComponent
     */
    template<typename T>
    std::shared_ptr<OptionListComponent<T>> AddMultiList(const std::string& text, int id, IOptionListMultiComponent<T>* interface)
    {
      return AddMultiList(text, id, interface, Strings::Empty);
    }

  private:
    //! callback Interface
    IGuiMenuBase* mInterface;
    //! Footer
    std::string mFooter;
    //! Menu initialized flag
    bool mMenuInitialized;

    //! Check if a reboot is pending. If not, send an infinite popup
    static void RebootPending(WindowManager* window);

    //! Relaunch
    static void Relaunch();

    //! Reboot
    static void Reboot();

    /*
     * IComponentListRow implementation
     */
    void ComponentListRowSelected(int id) override;
};
