//
// Created by bkg2k on 08/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <guis/menus/GuiMenuBase.h>

// Forward declaration
class SystemManager;
template<class T> class OptionListComponent;
class SwitchComponent;

class GuiMenuVirtualSystemPerGenre : public GuiMenuBase
{
  public:
    /*!
     * @brief Default constructor
     * @param window Global window
     */
    explicit GuiMenuVirtualSystemPerGenre(WindowManager& window);

    //! Destructor
    ~GuiMenuVirtualSystemPerGenre() override;

  private:
    //! Switch component per genre
    std::map<std::shared_ptr<SwitchComponent>, std::string> mComponents;
    //! Original values
    std::map<std::string, bool> mOriginalValues;
};



