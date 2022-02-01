//
// Created by Bkg2k on 04/02/2020.
//
#pragma once

class FileData;

class IFilter
{
  public:
    //! Destructor
    virtual ~IFilter() = default;

    /*!
     * @brief Filter a FileData entry
     * @param file FileData to filter
     * @return Return true to validate this entry, false to ignore it
     */
    virtual bool ApplyFilter(const FileData& file) const = 0;
};