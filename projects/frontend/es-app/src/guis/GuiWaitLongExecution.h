//
// Created by bkg2k on 26/10/2020.
//
#pragma once

#include <guis/Gui.h>
#include <utils/os/system/Thread.h>
#include <components/BusyComponent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <Renderer.h>

// Forward declaration
template<typename intype, typename outtype> class GuiWaitLongExecution;

/*!
 * @brief Task execution interface
 * @tparam intype In type
 * @tparam outtype Out type
 */
template<typename intype, typename outtype> class ILongExecution
{
  public:
    virtual outtype Execute(GuiWaitLongExecution<intype, outtype>& from, const intype& parameter) = 0;

    virtual void Completed(const intype& parameter, const outtype& result) = 0;
};

template<typename intype, typename outtype> class GuiWaitLongExecution : public Gui, public Thread, private ISynchronousEvent
{
  public:
    explicit GuiWaitLongExecution(WindowManager& window, ILongExecution<intype, outtype>& executor)
      : Gui(window)
      , mExecutor(executor)
      , mParameter()
      , mBusyAnim(window)
      , mSender(this)
    {
      // Configura animation
      mBusyAnim.setSize(Renderer::Instance().DisplayWidthAsFloat(), Renderer::Instance().DisplayHeightAsFloat());
    }

    /*!
     * @brief Destructor
     */
    ~GuiWaitLongExecution() override
    {
      Stop();
    }

    /*!
     * @brief Start task execution
     * @param parameter Input parameter
     * @param text initial text
     * @return This
     */
    GuiWaitLongExecution* Execute(const intype& parameter, const std::string& text)
    {
      // Do not restart if it's already started
      Join();

      // Update text
      mBusyAnim.setText(text);
      // Store parameter
      mParameter = parameter;

      // Start execution
      Start("WaitCommand");

      return this;
    }

    /*!
     * @brief Set anumation text
     * @param text text to set
     */
    void SetText(const std::string& text) { mBusyAnim.setText(text); }

    /*!
     * @brief Update components
     * @param deltaTime delta ms from the previous frame
     */
    void Update(int deltaTime) override
    {
      mBusyAnim.Update(deltaTime);
    }

    /*!
     * @brief Draw
     * @param parentTrans Parent transformation
     */
    void Render(const Transform4x4f& parentTrans) override
    {
      Transform4x4f trans = parentTrans * getTransform();
      Renderer::SetMatrix(trans);

      Renderer::DrawRectangle(0.f, (Renderer::Instance().DisplayHeightAsFloat() - mBusyAnim.RealHeight() * 1.6f) / 2.0f,
                         Renderer::Instance().DisplayWidthAsFloat(), mBusyAnim.RealHeight() * 1.6f, 0x00000080);

      mBusyAnim.Render(trans);
    }

    /*!
     * @brief This window is an overlay and does not hide underlying windows
     * @return Always true
     */
    bool IsOverlay() const override { return true; }

  protected:
    /*!
     * @brief Execute the given executor
     */
    void Run() override
    {
      // Execute the executor command & store the result
      mResult = mExecutor.Execute(*this, mParameter);
      // Notify the result can be sent back
      mSender.Call();
    }

  private:
    //! Executor reference
    ILongExecution<intype, outtype>& mExecutor;
    //! Input parameter reference
    intype mParameter;
    //! Result storage
    outtype mResult;

    //! Busy animation
    BusyComponent mBusyAnim;

    //! Event
    SyncronousEvent mSender;

    /*!
     * @brief Receive end of execution
     * @param event SDL2 event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override
    {
      (void)event;

      // Close gui
      Close();
      // Wait for thread to complete so that the Completed can restart a new process safely
      Join();
      // Give back the result in a synchronized way
      mExecutor.Completed(mParameter, mResult);
    }
};

