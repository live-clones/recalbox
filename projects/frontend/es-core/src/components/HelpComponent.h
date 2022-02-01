#pragma once

#include "components/base/Component.h"
#include "help/HelpStyle.h"
#include "ComponentGrid.h"

class ComponentGrid;

class HelpComponent : public Component
{
  public:
    /*!
     * @brief Default constructor
     * @param window Target window
     */
    explicit HelpComponent(WindowManager&window);

    /*!
     * @brief Called once per frame. Override to implement your own drawings.
     * Call your base::Update() to ensure animation and childrens are updated properly
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) final;

    /*!
     * @brief Called once per frame, after Update
     * Implement your own Render() to draw your own compponents or over-drawings
     * First, execute: 	Transform4x4f trans = (parentTrans * getTransform()).round();
     *                  Renderer::setMatrix(trans);
     * Then draw your components
     * Finally, call your base.Render(trans) to draw animations and childrens
     * @param parentTrans Transformation
     */
    void Render(const Transform4x4f& parent) final;

    void setOpacity(unsigned char opacity) final;

    void UpdateHelps();

  private:
    static constexpr int sPauseTime = 5000;

    //! Scrolling state for very long help systems
    enum class Scrolling
    {
      Initialize,    //!< Initialization frame
      PauseLeft,     //!< Paused left for X seconds
      ScrollToRight, //!< Scrolling to the right
      PauseRight,    //!< Paused right for X seconds
      ScrollToLeft,  //!< Scrolling to the left
    };

    //! Previous Help object
    Help mHelp;

    //! Grid component, 4 cells per Help item
  	ComponentGrid mGrid;

  	//! Scrolling
  	Scrolling mScrolling;

  	//! Scrolling time accumulator
  	int mScrollingTimeAccumulator;

  	//! Scrolling length
  	int mScrollingLength;

  	//! Scrolling offset
  	int mScrollingOffset;
};
