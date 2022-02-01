//
// Created by bkg2k on 08/02/2020.
//
#pragma once

#include <utils/math/Vector2f.h>

template<typename T> class ITextListComponentOverlay
{
  public:
    /*!
     * @brief Apply (draw) an overlay in the given item rectangle and adjust rectangle position/size
     * so that the text won't draw over the overlay if required
     * @param position Top/Left of the item rectangle
     * @param size  Width/Height of the item rectangle
     * @param data Linked data
     */
    virtual void OverlayApply(const Vector2f& position, const Vector2f& size, T& data, unsigned int& color) = 0;

    /*!
     * @brief Get the left offset (margin to the text) if any
     * @return left offset
     */
    virtual float OverlayGetLeftOffset() = 0;

    /*!
     * @brief Get the right offset (margin from text to right limit) if any
     * @return right offset
     */
    virtual float OverlayGetRightOffset() = 0;
};
