//
// Created by bkg2k on 22/12/2019.
//
#pragma once

/*!
 * @brief All item available on standard modern Pads
 */
enum class PadItems : short
{
  Invalid,       //!< Invalid!
  Up,            //!< DPad Up direction
  Right,         //!< DPad Right direction
  Down,          //!< DPad Down direction
  Left,          //!< DPad Left direction
  A,             //!< A Button
  B,             //!< B Button
  X,             //!< X Button
  Y,             //!< Y Button
  L1,            //!< L1 Button/Trigger
  R1,            //!< R1 Button/Trigger
  L2,            //!< L2 Button/Trigger
  R2,            //!< R2 Button/Trigger
  L3,            //!< L3 Button/Trigger
  R3,            //!< R3 Button/Trigger
  Start,         //!< Start button
  Select,        //!< Select button
  Hotkey,        //!< Hotkey (or Home) button
  J1Up,          //!< Joystick 1 (left) Up position
  J1Right,       //!< Joystick 1 (left) Right position
  J1Down,        //!< Joystick 1 (left) Down position
  J1Left,        //!< Joystick 1 (left) Left position
  J2Up,          //!< Joystick 2 (right) Uo positio
  J2Right,       //!< Joystick 2 (right) Right position
  J2Down,        //!< Joystick 2 (right) Down position
  J2Left,        //!< Joystick 2 (right) Left position
  HotKeyUp,      //!< Hotkey + DPad Up direction
  HotKeyRight,   //!< Hotkey + DPad Right direction
  HotKeyDown,    //!< Hotkey + DPad Down direction
  HotKeyLeft,    //!< Hotkey + DPad Left direction
  HotKeyA,       //!< Hotkey + A Button
  HotKeyB,       //!< Hotkey + B Button
  HotKeyX,       //!< Hotkey + X Button
  HotKeyY,       //!< Hotkey + Y Button
  HotKeyL1,      //!< Hotkey + L1 Button/Trigger
  HotKeyR1,      //!< Hotkey + R1 Button/Trigger
  HotKeyL2,      //!< Hotkey + L2 Button/Trigger
  HotKeyR2,      //!< Hotkey + R2 Button/Trigger
  HotKeyL3,      //!< Hotkey + L3 Button/Trigger
  HotKeyR3,      //!< Hotkey + R3 Button/Trigger
  HotKeyStart,   //!< Hotkey + Start button
  HotKeyJ1Up,    //!< Hotkey + Joystick 1 (left) Up position
  HotKeyJ1Right, //!< Hotkey + Joystick 1 (left) Right position
  HotKeyJ1Down,  //!< Hotkey + Joystick 1 (left) Down position
  HotKeyJ1Left,  //!< Hotkey + Joystick 1 (left) Left position
  HotKeyJ2Up,    //!< Hotkey + Joystick 2 (right) Uo positio
  HotKeyJ2Right, //!< Hotkey + Joystick 2 (right) Right position
  HotKeyJ2Down,  //!< Hotkey + Joystick 2 (right) Down position
  HotKeyJ2Left,  //!< Hotkey + Joystick 2 (right) Left position
  __Count
};
