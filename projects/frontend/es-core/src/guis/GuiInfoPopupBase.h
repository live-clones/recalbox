//
// Created by xizor on 30/03/18.
//
#pragma once

#include "guis/Gui.h"
#include "components/NinePatchComponent.h"
#include "components/ComponentGrid.h"
#include "WindowManager.h"
#include "utils/math/Vectors.h"

class GuiInfoPopupBase : public Gui
{
  public:
    enum class Corner
    {
      TopLeft,
      TopRight,
      BottomRight,
      BottomLeft,
    };

    enum class PopupType
    {
      None,     //!< No icon
      Warning,  //!< Warning icon!
      Music,    //!< Music icon
      Help,     //!< Help icon
      Netplay,  //!< Netplay logo
      Recalbox, //!< Recalbox logo
      Pads,     //!< Joystick logo
      Reboot,   //!< Pending reboot
      Scraper,  //!< Background scraping
    };

    //! Default destructor
    ~GuiInfoPopupBase() override = default;

    //! Popup must be closed?
    bool TimeOut() const { return !mRunning; }

    //! Popup type
    PopupType Type() const { return mType; }

    /*!
     * @brief Set initial target offset, relative to the selected corner
     * @param offset target offset
     */
    void SetOffset(int offset) { mTargetOffset = offset; }

    /*!
     * @brief Construct & initialize components
     */
    void Initialize();

    /*!
     * @brief Check if this instance is self processed and must not be deleted by the window manager
     */
    bool SelfProcessed() const { return mSelfProcessed; }

    /*
     * Component implementation
     */

    void Update(int delta) override;
    void Render(const Transform4x4f& parentTrans) override;

  protected:
    /*!
     * @brief Protected constructor
     * @param window Window manager
     * @param duration Popup duration
     * @param icon Icon type
     * @param gridWidth inner Grid width
     * @param gridHeight inner Grid height
     * @param widthRatio Ratio applied to the popup width
     */
    GuiInfoPopupBase(WindowManager& window, bool selfProcessed, int duration, PopupType icon, int gridWidth, int gridHeight, float widthRatio);

    /*!
     * @brief Stop this popup immediately
     */
    inline void stop() { mRunning = false; };

    /*!
     * @brief Allow inherited classes to acces Grid
     * @return Grid component
     */
    ComponentGrid& Grid() { return mGrid; }

    //! Check if the class is initialized
    bool Initialized() const { return mInitialized; }

    /*!
     * @brief Inherited class must use this method to add their components into the grid
     * @param window Window manager
     * @param grid  Component grid
     * @param maxWidth max width
     * @param maxHeight max height
     * @return Real height
     */
    virtual float AddComponents(WindowManager& window, ComponentGrid& grid, float maxWidth, float maxHeight, int paddingX, int paddingY) = 0;

    /*!
     * @brief Slide target offset from the given amount of pixels
     * @param offset
     */
    void SlideOffset(int size);

  private:
    ComponentGrid mGrid;
    NinePatchComponent mFrame;
    unsigned int mFrameColor;
    PopupType mType;
    Corner mCorner;
    int mTargetOffset;
    int mDuration;
    int maxAlpha;
    int mStartTime;
    float mWidthRatio;
    bool mRunning;
    bool mInitialized;
    bool mSelfProcessed;
};