//
// Created by bkg2k on 19/11/2019.
//

#include "ApplicationWindow.h"
#include "utils/Log.h"
#include "RotationManager.h"

bool ApplicationWindow::ProcessInput(const InputCompactEvent& event)
{
  if (event.IsKeyboard() && event.KeyUp() && (event.RawEvent().Code() == SDLK_F4))
  {
    WindowManager::ProcessInput(event); // Force window wakeup
    mClosed = true;
    return true;
  }
  const InputCompactEvent& rotated = RotationManager::ShouldRotateFrontendControls() ? InputCompactEvent::Rotate(event) : event;
  if (WindowManager::ProcessInput(rotated)) return true;
  return mViewController.ProcessInput(rotated);
}

void ApplicationWindow::Rotate(RotationType rotation) {
  {LOG(LogDebug) << "[ApplicationWindow] Starting rotation"; };
  Renderer::Instance().Rotate(rotation);
  {LOG(LogDebug) << "[ApplicationWindow] Finalizing windows"; };
  this->Finalize();
  {LOG(LogDebug) << "[ApplicationWindow] Init windows with renderer size"; };
  this->Initialize(Renderer::Instance().RealDisplayWidthAsInt(),Renderer::Instance().RealDisplayHeightAsInt());
};


void ApplicationWindow::Update(int deltaTime)
{
  mViewController.Update(deltaTime);
  WindowManager::Update(deltaTime);
}

void ApplicationWindow::Render(Transform4x4f& transform)
{
  mViewController.Render(transform);
  WindowManager::Render(transform);
}

bool ApplicationWindow::UpdateHelpSystem()
{
  if (!WindowManager::UpdateHelpSystem())
    mViewController.updateHelpPrompts();
  return true;
}
