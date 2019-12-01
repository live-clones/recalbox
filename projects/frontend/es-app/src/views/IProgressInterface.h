//
// Created by bkg2k on 21/11/2019.
//
#pragma once

class IProgressInterface
{
  public:
    /*!
     * @brief Set maximum value of the progress indicator
     * @param maximum maximum value
     */
    virtual void SetMaximum(int maximum) = 0;

    /*!
     * @brief Update the progress indicator vith the current value
     * @param value curren tprogress value
     */
    virtual void SetProgress(int value) = 0;

    /*!
     * @brief Increment the Progress value by 1.
     */
    virtual void Increment() = 0;
};
