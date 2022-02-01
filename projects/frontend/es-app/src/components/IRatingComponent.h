//
// Created by bkg2k on 28/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class IRatingComponent
{
  public:
    virtual void RatingChanged(int id, float value) = 0;
};
