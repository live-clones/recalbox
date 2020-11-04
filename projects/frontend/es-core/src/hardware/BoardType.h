//
// Created by bkg2k on 02/11/2020.
//
#pragma once

//! Board model/generation
enum class BoardType
{
    // Undetected
    UndetectedYet, // Not yet detected
    Unknown,       // Unknown hardware
    // RaspberryPi
    Pi0,           // Pi 0, 0W
    Pi1,           // Pi 1, A, B, A+, B+
    Pi2,           // Pi 2B
    Pi3,           // Pi 3B
    Pi3plus,       // Pi 3B+
    Pi4,           // Pi 4B
    UnknownPi,     // Unknown Pi with higher revisions
    // Odroid
    OdroidAdvanceGo2, // Odroid advance go 2
    // PC
    PCx86,
    PCx64,
};
