//
// Created by bkg2k on 19/11/2019.
//

#include "ApplicationWindow.h"

void ApplicationWindow::ProcessInput(const InputCompactEvent& event)
{
  if (event.IsKeyboard() && event.KeyUp() && (event.RawEvent().Code() == SDLK_F4))
    mClosed = true;
  else
  {
    if (!mViewController.ProcessInput(event))
      Window::ProcessInput(event);
  }
}

void ApplicationWindow::Update(int deltaTime)
{
  mViewController.update(deltaTime);
  Window::Update(deltaTime);
}

void ApplicationWindow::RenderAllGraphics()
{
  Transform4x4f transform = Transform4x4f::Identity();

  mViewController.render(transform);
  Window::Render(transform);
}
