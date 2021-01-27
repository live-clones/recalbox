//
// Created by bkg2k on 25/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

template<typename T> class IOptionListMultiComponent
{
  public:
    /*!
     * @brief Called when an editable component has been updated
     * @param id Component id
     * @param text New value set
     */
    virtual void OptionListMultiComponentChanged(int id, const std::vector<T>& value) = 0;
};
