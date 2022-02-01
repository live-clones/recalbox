//
// Created by bkg2k on 24/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class ISliderComponent
{
  public:
    virtual void SliderMoved(int id, float value) = 0;
};
