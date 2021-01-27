//
// Created by bkg2k on 22/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class IEditableComponent
{
  public:
    /*!
     * @brief Called when an editable component has been updated
     * @param id Component id
     * @param text New text
     */
    virtual void EditableComponentTextChanged(int id, const std::string& text) = 0;
};
