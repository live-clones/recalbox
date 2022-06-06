//
// Created by bkg2k on 06/06/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class FileData;

class IParser
{
  public:
    //! Destructor
    virtual ~IParser() = default;

    /*!
     * @brief Parse a FileData entry
     * @param file FileData to parse
     */
    virtual void Parse(FileData& file) = 0;
};