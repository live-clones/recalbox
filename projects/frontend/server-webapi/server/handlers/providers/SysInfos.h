//
// Created by bkg2k on 06/04/2020.
//
#pragma once

#include <utils/datetime/DateTime.h>
#include <utils/json/JSONBuilder.h>
#include <mqtt/MqttClient.h>
#include "utils/os/system/Thread.h"

class SysInfos : public Thread
{
  private:
    enum CpuDataType
    {
      User,    //!< Time spent with normal processing in user mode.
      Nice,    //!< Time spent with niced processes in user mode.
      System,  //!< Time spent running in kernel mode.
      Idle,    //!< Time spent in vacations twiddling thumbs.
      IOWait,  //!< Time spent waiting for I/O to completed. This is considered idle time too.
      Irq,     //!< Time spent serving hardware interrupts. See the description of the intr line for more details.
      SoftIrq, //!< Time spent serving software interrupts.
      Steal,   //!< Time stolen by other operating systems running in a virtual environment.
      Guest,   //!< Time spent for running a virtual CPU or guest OS under the control of the kernel.
      __Count
    };

    //! Storage size in bit
    static constexpr int sStorageBits = 6;
    //! Storage size
    static constexpr int sStorageSize = 1 << sStorageBits;
    //! Storage mask
    static constexpr int sStorageMask = sStorageSize - 1;

    //! Max cpu stored
    static constexpr int sMaxCPU = 16;

  public:
    // Sub type
    typedef float FloatStorage[sStorageSize];

    // Sub type
    typedef long long LongStorage[sStorageSize];

    enum class Platform
    {
      Unknown,
      RaspberryPi,
      Odroid,
      PC,
    };

  private:
    //! MQTT client reference
    MqttClient mMqttClient;

    //! Platform type
    Platform mPlatform;

    //! Cpu names
    std::string mCpuNames[sMaxCPU];

    // data
    struct DataBag
    {
      //! Cpu count
      int CpuCount;
      //! Cpu storage
      FloatStorage Cpu[sMaxCPU];
      //! Cpu data
      long long CpuData[sMaxCPU][CpuDataType::__Count];

      //! Free Ram storage
      long long FreeMemory[sStorageSize];
      //! Available ram storage
      long long AvailableMemory[sStorageSize];
      //! Ram total
      long long TotalMemory;

      //! T° storage
      float Temperature[sStorageSize];

      //! Storage current index
      unsigned int StorageIndex;

      //! Reset current databag
      void Reset();
    } mData;

    /*!
     * @brief Read and store CPU names
     */
    void ReadCPUNames();

    /*!
     * @brief Get CPU data
     */
    void GetCpu();

    /*!
     * @brief Get temperature data
     */
    void GetTemperature();

    /*!
     * @brief Get memory data
     */
    void GetMemory();

    /*!
     * @brief Refresh all data
     */
    void RefreshAll();

    /*!
     * @brief Get platform type from name
     * @param platformName Platform name
     * @return Platform type
     */
    static Platform GetPlatformFrom(const std::string& platformName);

    /*!
     * @brief Get platform full name from type
     * @param platform Platform type
     * @return Platform type
     */
    static const char* GetPlatformFrom(Platform platform);

    /*!
     * @brief Get data array shifted left bt current storage index
     * so that oldst value appear first and newer value appeay last
     * @tparam T Underlying type
     * @param destination Destination array
     * @param source Source array
     */
    template<typename T> void GetData(T* destination, const T* source) const
    {
      memcpy(destination, &source[mData.StorageIndex], (sStorageSize - mData.StorageIndex) * sizeof(T));
      if (mData.StorageIndex != 0)
        memcpy(&destination[sStorageSize - mData.StorageIndex], source, mData.StorageIndex * sizeof(T));
    }

  protected:
    /*!
     * Calls the inheriting object's Run() method.
     * @note set fIsRunning false to exit
     */
    void Run() override;

  public:
    /*!
     * @brief Default constructor
     */
    SysInfos();

    /*
     * Getters
     */

    //! Get measure count
    static int MeasureCount() { return sStorageSize; }

    //! Get Cpu count
    int CpuCount() const { return mData.CpuCount; }

    //! Get all measures of the given cpu
    void CpuConsumption(int cpu, FloatStorage& percents) const;

    //! Get cpu names
    const std::string& CpuName(int cpu) const { return mCpuNames[cpu]; }

    //! Get platform name
    const char* SystemPlatform() const { return GetPlatformFrom(mPlatform); }

    //! Get total memory
    long long TotalMemory() const { return mData.TotalMemory; }

    //! Get all memory measures
    void Memory(LongStorage& free, LongStorage& available) const;

    //! Get all memory measures
    void Temperature(FloatStorage& temperature) const;

    /*
     * Serializers
     */

    /*!
     * @brief Build Platform object
     * @return Platform object
     */
    JSONBuilder BuildPlatformObject();

    /*!
     * @brief Build CPU object
     * @return CPU object
     */
    JSONBuilder BuildCpuObject(bool all);

    /*!
     * @brief Build RAM object
     * @return RAM object
     */
    JSONBuilder BuildMemoryObject(bool all);

    /*!
     * @brief Build Temperature object
     * @return
     */
    JSONBuilder BuildTemperatureObject(bool all);

    /*!
     * @brief Send all cpu/ram/t° update object
     * @param timestamp Timestamp of objects
     */
    void SendMqttObjects(const DateTime& timestamp);
};
