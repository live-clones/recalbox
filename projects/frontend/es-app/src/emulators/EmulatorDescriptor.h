//
// Created by bkg2k on 01/11/2019.
//
#pragma once

#include <utils/Log.h>

class EmulatorDescriptor
{
  public:
    //! Compatibility information
    enum class Compatibility
    {
      Unknown, //!< Unknown
      High   , //!< Perfect or near perfect
      Good   , //!< Some games don't work at all or have issues
      Average, //!< Expect to run half of the games
      Low    , //!< only a few games run well and w/o issues
    };

    //! Speed information
    enum class Speed
    {
      Unknown, //!< Unknown
      High   , //!< Perfect or near perfect
      Good   , //!< Some games will not run at full speed or may have slowdowns
      Average, //!< Expect around half of the games running at a decent framerate
      Low    , //!< Only a few games are playable
    };

    //! Maximum core per emulator
    static constexpr int sMaximumCores = 10;

    //! Default constructor
    EmulatorDescriptor()
      : EmulatorDescriptor("")
    {
    }

    /*!
     * @brief Constructor
     * @param emulator Emulator name
     */
    explicit EmulatorDescriptor(const std::string& emulator)
      : mEmulator(emulator)
      , mCoreCount(0)
    {
      for(int i = sMaximumCores; --i >= 0; )
        mCores[i].Reset();
    }

    //! Get emulator name
    const std::string& Name() const { return mEmulator; }

    //! Get core count
    int CoreCount() const { return mCoreCount; }

    //! Has at least one core?
    bool HasAny() const { return mCoreCount != 0; }

    /*!
     * @brief Check if the emulator has a core matching the given name
     * @param name Core name
     * @return True of the emulator has this core, fals eotherwise
     */
    bool HasCore(const std::string& name) const
    {
      for(int i=mCoreCount; --i>=0; )
        if (name == mCores[i].mName)
          return true;
      return false;
    }

    //! Get core name
    const std::string& CoreNameAt(int index) const { return CoreAt(index).mName; }
    //! Get core priority
    unsigned char CorePriorityAt(int index) const { return CoreAt(index).mPriority; }
    //! Get core Extensions
    const std::string& CoreExtensions(int index) const { return CoreAt(index).mExtensions; }
    //! Get core Netplay
    bool CoreNetplay(int index) const { return CoreAt(index).mNetplay; }
    //! Get core Softpatching
    bool CoreSoftpatching(int index) const { return CoreAt(index).mSoftpatching; }
    //! Get core Speed
    Speed CoreSpeed(int index) const { return CoreAt(index).mSpeed; }
    //! Get core Compatibility
    Compatibility CoreCompatibility(int index) const { return CoreAt(index).mCompatibility; }
    //! Get core available on CRT
    bool CoreCrtAvailable(int index) const { return CoreAt(index).mCRTAvailable; }

    /*!
     * @brief Add core
     * @param name Name (file name part)
     * @param priority Priority (lowest value = highest priority)
     * @param extensions File extensions
     * @param netplay Netplay support?
     * @param compatibility Compatibility evaluation
     * @param speed Speed evaluation
     */
    void AddCore(const std::string& name,
                 int priority,
                 const std::string& extensions,
                 bool netplay,
                 const std::string& compatibility,
                 const std::string& speed,
                 bool softpatching,
                 bool crtAvailable)
    {
      if (mCoreCount < sMaximumCores)
      {
        Core& core = mCores[mCoreCount++];
        core.mName = name;
        core.mExtensions = extensions;
        core.mPriority = priority;
        core.mCompatibility = ConvertCompatibility(compatibility);
        core.mSpeed = ConvertSpeed(speed);
        core.mNetplay = netplay;
        core.mSoftpatching = softpatching;
        core.mCRTAvailable = crtAvailable;
      }
      else { LOG(LogError) << "[Emulator] Core " << name << " cannot be added to emulator " << mEmulator; }
    }

  private:
    //! Core structure
    struct Core
    {
      std::string mName;            //!< Core name (file name)
      std::string mExtensions;      //!< Supported extensions
      int mPriority;                //!< Core priority
      Compatibility mCompatibility; //!< Compatibility rate
      Speed mSpeed;                 //!< Average speed
      bool mNetplay;                //!< Netplay compatible?
      bool mSoftpatching;           //!< Softpathing compatible?
      bool mCRTAvailable;           //!< Available on CRT?

      //! Constructor
      Core()
        : mPriority(0)
        , mCompatibility(Compatibility::Unknown)
        , mSpeed(Speed::Unknown)
        , mNetplay(false)
        , mSoftpatching(false)
        , mCRTAvailable(false)
      {
      }

      //! Reset default values
      void Reset()
      {
        mName.clear();
        mExtensions.clear();
        mPriority = 255;
        mCompatibility = Compatibility::Unknown;
        mSpeed = Speed::Unknown;
        mNetplay = false;
        mSoftpatching = false;
        mCRTAvailable = false;
      }
    };

    //! Emulator name
    std::string mEmulator;
    //! Core specifications
    Core mCores[sMaximumCores];
    //! Core count
    int mCoreCount;

    const Core& CoreAt(int index) const { return (unsigned int)index < (unsigned int)mCoreCount ? mCores[index] : mCores[0]; }

    /*!
     * @brief Convert compatibility string to compatibility enum
     * @param compatibility compatibility string
     * @return compatibility enum
     */
    static Compatibility ConvertCompatibility(const std::string& compatibility)
    {
      Compatibility result = Compatibility::Unknown;
      if      (compatibility == "unknown"  ) result = Compatibility::Unknown;
      else if (compatibility == "high"     ) result = Compatibility::High;
      else if (compatibility == "good"     ) result = Compatibility::Good;
      else if (compatibility == "average"  ) result = Compatibility::Average;
      else if (compatibility == "low"      ) result = Compatibility::Low;
      else { LOG(LogError) << "[SystemDescriptor] Unknown compatibility type " << compatibility << " !"; }
      return result;
    }

    /*!
     * @brief Convert speed string to speed enum
     * @param speed speed string
     * @return speed enum
     */
    static Speed ConvertSpeed(const std::string& speed)
    {
      Speed result = Speed::Unknown;
      if      (speed == "unknown"  ) result = Speed::Unknown;
      else if (speed == "high"     ) result = Speed::High;
      else if (speed == "good"     ) result = Speed::Good;
      else if (speed == "average"  ) result = Speed::Average;
      else if (speed == "low"      ) result = Speed::Low;
      else { LOG(LogError) << "[SystemDescriptor] Unknown Speed type " << speed << " !"; }
      return result;
    }
};

