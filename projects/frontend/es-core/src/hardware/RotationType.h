#pragma once

#include <string>

//! Rotation type
enum class RotationType : unsigned int
{
  None,
  Left,
  Upsidedown,
  Right,
};

//! Rotation capabilities of the board
typedef struct {
  bool canRotate;
  RotationType defaultRotationWhenTate;
  bool rotateControls;
  bool autoRotateGames;
} RotationCapability;

class RotationUtils {
public:

  static RotationType FromString(const std::string& value){
    if(value == "Left") return RotationType::Left;
    if(value == "Right") return RotationType::Right;
    if(value == "Upsidedown") return RotationType::Upsidedown;
    return RotationType::None;
  }

  static std::string StringValue(RotationType value){
    switch(value){
      case(RotationType::Left): return "Left";
      case(RotationType::Right): return "Right";
      case(RotationType::Upsidedown): return "Upsidedown";
      case(RotationType::None): return "None";
    }
    return "None";
  }

  static RotationType FromAngle(const std::string& angle){
    if(angle == "90") return RotationType::Left;
    if(angle == "270") return RotationType::Right;
    if(angle == "180") return RotationType::Upsidedown;
    return RotationType::None;
  }

  static RotationType FromUint(const unsigned int value){
    if(value < 4) return (RotationType)value;
    return RotationType::None;
  }

};