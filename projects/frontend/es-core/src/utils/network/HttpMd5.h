//
// Created by bkg2k on 02/06/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/network/Http.h>
#include <utils/hash/Md5.h>

class HttpMd5 : public Http
{
  public:
    //! Default constructor
    ~HttpMd5() override = default;

    //! Get md5
    std::string Md5() const { return mMd5.hexdigest(); }

  private:
    //! Md5 calculator
    MD5 mMd5;

    /*!
     * @brief CURL callback when receiving data, class instance compatible
     * @param data Data pointer
     * @param length Data length
     * @return Must return length when fully processed. Any other value means an error
     */
    void DataReceived(const char* data, int length) final { mMd5.update(data, length); }

    /*!
     * @brief Called when data start to download
     */
    void DataStart() final { mMd5.reset(); }

    /*!
     * @brief Called when all data are dowloaded
     */
    void DataEnd() final { mMd5.finalize(); }
};
