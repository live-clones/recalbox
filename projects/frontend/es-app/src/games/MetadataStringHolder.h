//
// Created by bkg2k on 14/11/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <utils/storage/Array.h>
#include <utils/storage/HashMap.h>
#include <utils/os/system/Mutex.h>

class MetadataStringHolder
{
  public:
    //! 32bits string index
    typedef int Index32;
    //! 16bits string index
    typedef short Index16;
    //! 8bits string index
    typedef char Index8;

    /*!
     * @brief Constructor
     * @param capacity Initial capacity
     * @param granularity Granularity
     */
    MetadataStringHolder(int capacity, int granularity);

    /*!
     * @brief Initialize all
     */
    void Initialize();

    /*!
     * @brief Finalize temporary items, let other item living
     */
    void Finalize();

    /*!
     * @brief Add a string and return its index in 32bit format
     * @param newString String to add
     * @return String index
     */
    Index32 AddString32(const std::string& newString);

    /*!
     * @brief Add a string and return its index in 32bit format
     * @param newString String to add
     * @return String index
     */
    Index16 AddString16(const std::string& newString) { return (Index16)AddString32(newString); }

    /*!
     * @brief Add a string and return its index in 32bit format
     * @param newString String to add
     * @return String index
     */
    Index8 AddString8(const std::string& newString) { return (Index8)AddString32(newString); }


    /*!
     * @brief Get a string from its index
     * @param index String index
     * @return String
     */
    std::string GetString(Index32 index);

    //! Get storage size
    int StorageSize() const { return mMetaString.Capacity() + mIndexes.ByteSize(); }

    //! Get storage size
    int ObjectCount() const { return mIndexes.Count(); }

    //! Is the given index valid?
    bool IsValid(int index) const { return index < mIndexes.Count(); }

  private:
    //! Synchronizer
    Mutex mSyncher;
    //! String containing all substrings
    Array<char> mMetaString;
    //! Substring indexes to char indexes
    Array<Index32> mIndexes;
    //! Temporary dictionnary string => indexes
    HashMap<std::string, Index32> mStringToIndexes;
};


