//
// Created by bkg2k on 19/11/2019.
//

#include "ApplicationWindow.h"

bool ApplicationWindow::ProcessInput(const InputCompactEvent& event)
{
  if (event.IsKeyboard() && event.KeyUp() && (event.RawEvent().Code() == SDLK_F4))
  {
    Window::ProcessInput(event); // Force window wakeup
    mClosed = true;
    return true;
  }

  if (Window::ProcessInput(event)) return true;
  return mViewController.ProcessInput(event);
}

void ApplicationWindow::Update(int deltaTime)
{
  mViewController.Update(deltaTime);
  Window::Update(deltaTime);
}

void ApplicationWindow::Render(Transform4x4f& transform)
{
  mViewController.Render(transform);
  Window::Render(transform);
}
