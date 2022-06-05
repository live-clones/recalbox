//
// Created by bkg2k on 22/05/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

enum class PatronAuthenticationResult
{
  Unknown,      //!< Undetermined yet
  NoPatron,     //!< User is not a patron
  Patron,       //!< User IS a patron
  FormerPatron, //!< User is a former patron but he's no longer one
  Invalid,      //!< Key is invalid or the user's never been a Recalbox patron
  NetworkError, //!< No network available
  HttpError,    //!< Http Error
  ApiError,     //!< APi wrong format
};