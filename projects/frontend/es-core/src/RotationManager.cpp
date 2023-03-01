//
// Created by digitalLumberjack on 10/03/23.
//

#include "RotationManager.h"
#include "hardware/Board.h"
#include "recalbox/BootConf.h"
#include "Renderer.h"

bool RotationManager::ShouldRotateTateEnter(RotationType& rotationType)
{
  // We will rotate the tate system only if:
  // - the board can rotate
  // - there is no Left or Right rotation yet
  // - the system Tate is configured to be rotated
  if(Board::Instance().GetRotationCapabilities().canRotate
    && !Renderer::Instance().IsRotatedSide()
    && RecalboxConf::Instance().GetCollectionTateRotateSystemView()){
      rotationType = static_cast<RotationType>(RecalboxConf::Instance().GetCollectionTateRotation());
      return true;
  }
  return false;
}

bool RotationManager::ShouldRotateTateExit(RotationType& rotationType)
{
  // We will rotate back the tate system only if:
  // - the board can rotate
  // - there is Left or Right rotation
  // - the system Tate is configured to be rotated
  // - the system wide rotation is not left or right
  if(Board::Instance().GetRotationCapabilities().canRotate
     && Renderer::Instance().IsRotatedSide()
     && RecalboxConf::Instance().GetCollectionTateRotateSystemView()
     && (GetSystemRotation() == RotationType::None || GetSystemRotation() == RotationType::Upsidedown)){
    rotationType = RotationType::None;
    return true;
  }
  return false;
}

RotationType RotationManager::ShouldRotateGame(const FileData& game)
{
  // We will rotate the game if:
  // - the game is a vertical game
  // - the board can rotate
  // - the rotate game option is set
  if(game.Metadata().Rotation() != RotationType::None) {
    const RotationCapability cap = Board::Instance().GetRotationCapabilities();
    bool rotate = cap.canRotate && RecalboxConf::Instance().GetCollectionTateRotateGames();
    if (rotate) {
      {LOG(LogDebug) << "[RotationManager] Rotate game will be set ON";}
      return cap.defaultRotationWhenTate != RotationType::None ? cap.defaultRotationWhenTate : game.Metadata().Rotation();
    }
  }
  return RotationType::None;
}

bool RotationManager::ShouldRotateFrontendControls()
{
  // We will rotate the controls if:
  // - the board can rotate
  // - the board is rotated for tate or system-wide
  const RotationCapability cap = Board::Instance().GetRotationCapabilities();
  bool rotate = cap.canRotate && cap.rotateControls && Renderer::Instance().IsRotatedSide();
  if (rotate) { LOG(LogDebug) << "[RotationManager] Rotate controls ON"; };
  return rotate;
}

bool RotationManager::ShouldRotateGameControls(const FileData& game)
{
  // We will rotate the controls if:
  // - the game is a vertical game
  // - the board can rotate
  // - the board is rotated for tate or system-wide
  const RotationCapability cap = Board::Instance().GetRotationCapabilities();
  const RotationType gameRotation = ShouldRotateGame(game);
  bool rotate = (gameRotation == RotationType::Left ||gameRotation == RotationType::Right) && cap.rotateControls;
  if (rotate) { LOG(LogDebug) << "[RotationManager] Rotate game controls ON"; };
  return rotate;
}

RotationType RotationManager::GetSystemRotation()
{
  return BootConf::Instance().GetRotation();
}