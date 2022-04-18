//
// Created by bkg2k on 31/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <utils/datetime/DateTime.h>
#include <utils/storage/HashMap.h>

class DnsClient
{
  public:
    //! Get Txt record content from the given domain
    std::string GetTxtRecord(const std::string& domain);

    //! Get A record content from the given domain
    std::string GetARecord(const std::string& domain);

  private:
    //! Cache entry
    class Entry
    {
      public:
        // Record cache structure
        class RecordCache
        {
          public:
            //! Default constructor
            RecordCache() = default;

            /*!
             * @brief Set cache data
             * @param data Data to cache. Timestamped now.
             */
            void Set(const std::string& data)
            {
              mRecordContent  = data;       // Recoprd data
              mRecordDateTime = DateTime(); // Now
            }

            //! Get data
            const std::string& Data() const { return mRecordContent; }
            //! Get timestamp
            const DateTime& TimeStamp() const { return mRecordDateTime; }

            //! Valid cache?
            bool IsValid() const { return !mRecordContent.empty() && (DateTime() - mRecordDateTime).TotalHours() < 8; }

          private:
            //! Record's cached data
            std::string mRecordContent;
            //! Record's cache datetime
            DateTime mRecordDateTime;
        };

        //! Default constructor
        Entry() = default;

        //! Set A record
        void SetARecord(const std::string& data) { mARecord.Set(data); }
        //! Set Txt record
        void SetTxtRecord(const std::string& data) { mTxtRecord.Set(data); }

        //! Get A record
        const RecordCache& ARecord() const { return mARecord; }
        //! Get Txt record
        const RecordCache& TxtRecord() const { return mTxtRecord; }

      private:
        //! A Record cache
        RecordCache mARecord;
        //! Txt Record cache
        RecordCache mTxtRecord;
    };

    //! Record cache
    HashMap<std::string, Entry> mCache;
};



