//
// Created by bkg2k on 06/01/23.
//

#include "GuiBluetoothDevices.h"
#include "components/ButtonComponent.h"
#include "components/MenuComponent.h"
#include <bluetooth/BluetoothListener.h>
#include <components/TextComponent.h>
#include <utils/locale/LocaleHelper.h>

GuiBluetoothDevices::GuiBluetoothDevices(WindowManager& windowManager)
  : Gui(windowManager)
  , mBackground(windowManager, Path(":/frame.png"))
  , mGrid(windowManager, Vector2i(4, 5))
  , mList(nullptr)
  , mMenuTheme(MenuThemeData::getInstance()->getCurrentTheme())
{
  addChild(&mBackground);
  addChild(&mGrid);

  mBackground.setImagePath(mMenuTheme->menuBackground.path);
  mBackground.setCenterColor(mMenuTheme->menuBackground.color);
  mBackground.setEdgeColor(mMenuTheme->menuBackground.color);

  BuildComponents();
  FillDeviceList();

  BluetoothListener::Instance().Register(this);
}

GuiBluetoothDevices::~GuiBluetoothDevices()
{
  BluetoothListener::Instance().Unregister(this);
}

// 4 x 5
// +-+-----------------------------------------------------------------+--------------------------------------------------------------+-+
// | |                                                         TITLE                                                                  | |  0 = Font height
// +-+-----------------------------------------------------------------+--------------------------------------------------------------+-+
// | |                                                                 |                                                              | |
// | |                    LIST                                         |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |  1 = remaining space
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// | |                                                                 |                                                              | |
// +-+-----------------------------------------------------------------+--------------------------------------------------------------+-+
// | |                                                         SPACER                                                                 | |  2 = 2%
// +-+-----------------------------------------------------------------+--------------------------------------------------------------+-+
// | |                                                         BUTTONS                                                                | |  3 = Button height
// +-+-----------------------------------------------------------------+--------------------------------------------------------------+-+
// | |                                                         SPACER                                                                 | |  4 = 2%
// +-+-----------------------------------------------------------------+--------------------------------------------------------------+-+
//  0                              1                                                                     2                             3
//  2%                       remaining space                                                       remaining space                     2%

void GuiBluetoothDevices::BuildComponents()
{
  // Title
  mTitle = std::make_shared<TextComponent>(mWindow, _("PAIR BLUETOOTH CONTROLLERS"), mMenuTheme->menuTitle.font, mMenuTheme->menuTitle.color, TextAlignment::Center);
  mGrid.setEntry(mTitle, Vector2i(1, 0), false, true, Vector2i(2, 1));

  // List
  mList = std::make_shared<ComponentList>(mWindow);
  mGrid.setEntry(mList, Vector2i(1, 1), true, true, Vector2i(1, 1));
  mList->setCursorChangedCallback([this](CursorState state) { (void) state; FillDetails(); });
  mList->setFocusLostCallback([this] { FillDetails(); });
  mList->setFocusGainedCallback([this] { FillDetails(); });

  // Details
  mDetails = std::make_shared<TextComponent>(mWindow, "", mMenuTheme->menuText.font, mMenuTheme->menuTitle.color, TextAlignment::Left);
  mDetails->setVerticalAlignment(TextAlignment::Top);
  mGrid.setEntry(mDetails, Vector2i(2, 1), false, true, Vector2i(1, 1));

  // Buttons
  std::vector<std::shared_ptr<ButtonComponent> > buttons;
  buttons.push_back(std::make_shared<ButtonComponent>(mWindow, _("CLOSE"), _("CLOSE"), [this] { Close(); }));
  mButtons = makeButtonGrid(mWindow, buttons);
  mGrid.setEntry(mButtons, Vector2i(1, 3), true, false, Vector2i(2, 1));

  const float height = Renderer::Instance().DisplayHeightAsFloat() * 0.75f;
  const float width = Renderer::Instance().DisplayWidthAsFloat() * 0.85f;
  setSize(width, height);
  setPosition((Renderer::Instance().DisplayWidthAsFloat() - width) / 2,
              (Renderer::Instance().DisplayHeightAsFloat() - height) / 2);
}

void GuiBluetoothDevices::onSizeChanged()
{
  mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

  const float titleVerticalPadding = (Renderer::Instance().DisplayHeightAsFloat() * 0.0637f);
  mGrid.setRowHeightPerc(0, (mTitle->getFont()->getLetterHeight() + titleVerticalPadding) / mSize.y());
  mGrid.setRowHeightPerc(1, 0.f);
  mGrid.setRowHeightPerc(2, 0.02f);
  mGrid.setRowHeightPerc(3, mButtons->getSize().y() / mSize.y());
  mGrid.setRowHeightPerc(4, 0.02f);

  mGrid.setColWidthPerc(0, 0.02f);
  mGrid.setColWidthPerc(1, 0.f);
  mGrid.setColWidthPerc(2, Renderer::Instance().Is240p() ? 0.01f : 0.f);
  mGrid.setColWidthPerc(3, 0.02f);

  mGrid.setSize(mSize);
}

void GuiBluetoothDevices::FillDetails()
{
  if (mDevices.empty() || Renderer::Instance().Is240p()) mDetails->setText("");
  else
  {
    DeviceStatus::Device& device = mDevices[mList->getCursorIndex()];
    String text;
    text.Append(_("Alias: ")).Append(device.Name)
        .Append('\n').Append(_("MAC: ")).Append(device.Address)
        .Append('\n').Append(_("Connected: ")).Append(device.Connected ? _("YES") : _("NO"))
        .Append('\n').Append(_("Trusted: ")).Append(device.Trusted ? _("YES") : _("NO"))
        .Append('\n').Append(_("Paired: ")).Append(device.Paired ? _("YES") : _("NO"))
        .Append('\n').Append(_("Blocked: ")).Append(device.Blocked ? _("YES") : _("NO"));
    mDetails->setText(text);
  }
}

void GuiBluetoothDevices::FillDeviceList()
{
  int currentPosition = mList->getCursorIndex();
  if (mList) mList->clear();
  ComponentListRow row;
  if (!mDevices.empty())
  {
    for (const auto& device : mDevices)
    {
      row.elements.clear();
      std::shared_ptr<Component> ed = std::make_shared<TextComponent>(mWindow, ComposeName(device), mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Left);
      row.addElement(ed, true);
      mList->addRow(row, false, true);
    }
    if (currentPosition >= mList->Count()) currentPosition = mList->Count() - 1;
    mList->setCursorIndex(currentPosition);
  }
  else
  {
    std::shared_ptr<Component> ed = std::make_shared<TextComponent>(mWindow, _("NO DEVICE"), mMenuTheme->menuText.font, mMenuTheme->menuText.color, TextAlignment::Left);
    row.addElement(ed, true);
    mList->addRow(row, false, true);
  }
  mGrid.getCellAt(1, 1)->canFocus = !mDevices.empty();
}

String GuiBluetoothDevices::ComposeName(const DeviceStatus::Device& device)
{
  String result;
  // Connection status
  if      (device.Connected) result.Append("\uf1dd ");
  else if (device.Paired   ) result.Append("\uf1dc ");
  else                       result.Append("\uf1db ");
  // RSSI
  switch(device.RSSI)
  {
    case DeviceStatus::SignalStrength::Best:
    case DeviceStatus::SignalStrength::Good: result.Append("\uf1c8 "); break;
    case DeviceStatus::SignalStrength::Average: result.Append("\uf1c7 "); break;
    case DeviceStatus::SignalStrength::Bad: result.Append("\uf1c6 "); break;
    case DeviceStatus::SignalStrength::Awfull: result.Append("\uf1c5 "); break;
    case DeviceStatus::SignalStrength::Unknown:
    default: result.Append("\uf1c9 "); break;
  }
  result.Append(device.Name);
  return result;
}

void GuiBluetoothDevices::Render(const Transform4x4f& parentTrans)
{
  Gui::Render(parentTrans);

  Transform4x4f trans = parentTrans * getTransform();
  Renderer::SetMatrix(trans);
  Renderer::DrawRectangle(mGrid.getPosition().x() + mGrid.getColWidth(0), mGrid.getPosition().y() + mGrid.getRowHeight(0),
                          mGrid.getColWidth(1), mGrid.getRowHeight(1), 0x00000011);
  Renderer::DrawRectangle(mGrid.getPosition().x() + mGrid.getColWidth(0, 1), mGrid.getPosition().y() + mGrid.getRowHeight(0),
                          mGrid.getColWidth(2), mGrid.getRowHeight(1), 0x00000018);
}

