//
// Created by bkg2k on 03/11/2020.
//
#pragma once

//! Message types
enum class MessageTypes
{
    None,               //!< None - Invalid message
    HeadphonePluggedIn, //!< Headphone have been plugged in
    HeadphoneUnplugged, //!< Headphone have been unplugged
    PowerButtonPressed, //!< Power button pressed and released after an amount of time
    VolumeUpPressed,    //!< Go3 Volume button up pressed
    VolumeDownPressed,  //!< Go3 Volume button down pressed
    Resume,             //!< Hardware exited from suspend mode
};
