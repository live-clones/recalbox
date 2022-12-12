//
// Created by bkg2k on 21/11/2019.
//
#include <WindowManager.h>
#include <components/TextComponent.h>
#include <utils/sync/SyncMessageSender.h>
#include <views/crt/CrtResolutions.h>

#pragma once

class CrtView : public Gui
              , private ISyncMessageReceiver<void>
{
  public:
    enum CalibrationType {
      kHz15_60plus50Hz,
      kHz15_60Hz,
      kHz15_50Hz,
      kHz31,
    };

    /*!
     * @brief Constructor
     * @param window Main Window instance
     */
    explicit CrtView(WindowManager& window, CalibrationType calibrationType);

    //! Destructor
    ~CrtView() override;

    /*
     * IComponent implementation
     */

    /*!
     * @brief Called once per frame, after Update.
     * @param parentTrans Transformation
     */
    void Render(const Transform4x4f& parentTrans) override;

    //! Process input events
    bool ProcessInput(const InputCompactEvent& event) override;

    //! Help bar
    bool getHelpPrompts(Help& help) override;

  private:
    static constexpr CrtResolution sForced31khz[] =
    {
      CrtResolution::r480p,
      CrtResolution::r240p120Hz,
      CrtResolution::rNone
    };

    static constexpr CrtResolution sPALOnly[] =
    {
      CrtResolution::r288p,
      CrtResolution::r576i,
      CrtResolution::rNone
    };

    static constexpr CrtResolution sNTSCOnly[] =
    {
      CrtResolution::r240p,
      CrtResolution::r224p,
      CrtResolution::r480i,
      CrtResolution::rNone
    };

    static constexpr CrtResolution sPALNTSC[] =
    {
      CrtResolution::r240p,
      CrtResolution::r224p,
      CrtResolution::r480i,
      CrtResolution::r288p,
      CrtResolution::r576i,
      CrtResolution::rNone
    };

    //! Timing file path
    static constexpr const char* sTimingFile = "/boot/crt/timings.txt";

    //! Pattern image
    ImageComponent mPattern;

    //! Inner grid
    ComponentGrid mGrid;
    //! Horizontal offset text
    std::shared_ptr<TextComponent> mHorizontalOffsetText;
    //! Vertical offset text
    std::shared_ptr<TextComponent> mVerticalOffsetText;
    //! Viewport text
    std::shared_ptr<TextComponent> mViewportText;

    //! Synchronous event
    SyncMessageSender<void> mEvent;

    //! Configuration sequence
    const CrtResolution* mSequence;
    //! Sequence index
    int mSequenceIndex;

    //! Original config
    int mOriginalVOffset;
    int mOriginalHOffset;
    int mOriginalViewportWidth;

    //! Original resolution width
    int mOriginalWidth;
    //! Original resolution height
    int mOriginalHeight;

    //! Viewport ratio
    int mStep;

    //! Update viewport
    void UpdateViewport();

    //! Update position
    void UpdatePosition();

    //! Change resolution
    void SetResolution(CrtResolution resolution);

    //! Initialize all the view
    void Initialize();

    /*
     * Synchronous event
     */

    /*!
     * @brief Receive message from the CRT thread
     */
    void ReceiveSyncMessage() override;
};


