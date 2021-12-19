//
// Created by bkg2k on 19/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <hardware/crt/CrtAdapterType.h>

class ICrtInterface
{
  public:
    enum class HorizontalFrequency
    {
      KHz15, //!< 15 Khz
      KHz31, //!< 31 Khz
    };

    /*!
     * @brief Default constructor
     * @param automaticallyDetected True if the board has been automatically detected
     */
    explicit ICrtInterface(bool automaticallyDetected) : mAutomaticallyDetected(automaticallyDetected) {}

    //! Default destructor
    virtual ~ICrtInterface() = default;

    /*!
     * @brief Check if a CRT adpater is attached
     * @return True if a CRT adapter is attached, false otherwise
     */
    virtual bool IsCrtAdapterAttached() = 0;

    /*!
     * @brief Get CRT adapter
     * @return CRT adater
     */
    virtual CrtAdapterType GetCrtAdapter() = 0;

    /*!
     * @brief Check if the adapter supports 31khz
     * @return True if the adapter supports 31khz
     */
    virtual bool Has31KhzSupport() = 0;

    /*!
     * @brief Get horizontal frequency
     * @return Horitontal frequency
     */
    virtual HorizontalFrequency GetHorizontalFrequency() = 0;

    /*!
     * @brief Check if the adapter has forced 50hs support
     * @return True if the adapter support forcing 50hz mode, false otherwise
     */
    virtual bool HasForced50hzSupport() = 0;

    /*!
     * @brief Check if the adapter force 50hz mode
     * @return True if the adapter force 50hz mode, otherwize automatic mode
     */
    virtual bool MustForce50Hz() = 0;

    /*!
     * @brief Check if this board has been automatically detected
     * @return True of the board has been automatically detected, false otherwise
     */
    bool HasBeenAutomaticallyDetected() const { return mAutomaticallyDetected; }

  private:
    //! Automatically detected?
    bool mAutomaticallyDetected;
};
