#pragma once

#include "components/base/Component.h"
#include "components/ComponentGrid.h"
#include "components/BusyComponent.h"
#include <games/FileData.h>

class ComponentList;
class ImageComponent;
class RatingComponent;
class TextComponent;
class DateTimeComponent;
class ScrollableContainer;
class HttpReq;
class AnimatedImageComponent;

class ScraperSearchComponent : public Component
{
  public:
    explicit ScraperSearchComponent(WindowManager& window);

    bool ProcessInput(const InputCompactEvent& event) override;

    void Update(int deltaTime) override;

    void Render(const Transform4x4f& parentTrans) override;

    bool getHelpPrompts(Help& help) override;

    void onSizeChanged() override;

    void onFocusGained() override
    { mGrid.onFocusGained(); }

    void onFocusLost() override
    { mGrid.onFocusLost(); }

    void UpdateInfoPane(const FileData* game);

    /*!
     * @brief Set running state - display or not busy animation
     * @param running True to show the busy animation
     */
    void SetRunning(bool running) { mRunning = running; }

  private:
    ComponentGrid mGrid;
    BusyComponent mBusyAnim;

    std::shared_ptr<TextComponent> mResultName;
    std::shared_ptr<ScrollableContainer> mDescContainer;
    std::shared_ptr<TextComponent> mResultDesc;
    std::shared_ptr<ImageComponent> mResultThumbnail;

    std::shared_ptr<RatingComponent> mValueRating;
    std::shared_ptr<DateTimeComponent> mValueReleaseDate;
    std::shared_ptr<TextComponent> mValueDeveloper;
    std::shared_ptr<TextComponent> mValuePublisher;
    std::shared_ptr<TextComponent> mValueGenre;
    std::shared_ptr<TextComponent> mValuePlayers;

    std::shared_ptr<TextComponent> mLabelRating;
    std::shared_ptr<TextComponent> mLabelReleaseDate;
    std::shared_ptr<TextComponent> mLabelDeveloper;
    std::shared_ptr<TextComponent> mLabelPublisher;
    std::shared_ptr<TextComponent> mLabelGenre;
    std::shared_ptr<TextComponent> mLabelPlayers;

    bool mRunning;
};
