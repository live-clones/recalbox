//
// Created by bkg2k on 15/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <vector>
#include <string>
#include <input/Input.h>
#include <input/IInputChange.h>
#include <utils/math/Misc.h>
#include <utils/Strings.h>

class InputMapper : IInputChange
{
  public:
    //! Pad structure
    struct Pad
    {
      std::string Name; //!< Real pad name
      std::string UUID; //!< Pad uuid
      int Identifier;   //!< Incremental index for multiple same pads

      Pad() : Identifier(-1) {}

      Pad(const std::string& name, const std::string& uuid, int index)
        : Name(name)
        , UUID(uuid)
        , Identifier(index)
      {
      }

      void Set(const std::string& name, const std::string& uuid, int index)
      {
        Name = name;
        UUID = uuid;
        Identifier = index;
      }

      void Reset()
      {
        Name.clear();
        UUID.clear();
        Identifier = -1;
      }

      bool IsValid() const { return !Name.empty() && !UUID.empty(); }

      bool IsConnected() const { return !Name.empty() && !UUID.empty() && Identifier >= 0; }

      bool Equals(const Pad& to) const
      {
        return Name == to.Name &&
               UUID == to.UUID &&
               Identifier == to.Identifier;
      }

      bool Same(const Pad& than) const
      {
        return Name == than.Name &&
               UUID == than.UUID;
      }

      std::string AsString() const { return std::string(Name).append(1, '.').append(UUID).append(1, '.').append(Strings::ToString(Identifier)); }
    };

    //! Pad array
    typedef Pad PadArray[Input::sMaxInputDevices];
    //! Pad list
    typedef std::vector<Pad> PadList;

    //! Constructor
    explicit InputMapper(IInputChange* interface);

    //! Destructor
    ~InputMapper();

    /*!
     * @brief Compose numbered name at the given index, by counting same pads' name/uuid with lower indexes
     * @param padArray Pad array
     * @param index Index of the pad to get name from
     * @return Name or numbered name
     */
    std::string GetDecoratedName(int index);

    /*!
     * @brief Lookup the given pad in the connected list
     * @param pad Pad to lookup
     * @return Pad index or -1 if not found
     */
    int LookupConnectedDevice(const Pad& pad) { return LookupDevice(mConnected, pad); }

    /*!
     * @brief Lookup the given pad in the unconnected list
     * @param pad Pad to lookup
     * @return Pad index or -1 if not found
     */
    int LookupUnconnectedDevice(const Pad& pad)  { return LookupDevice(mUnconnected, pad); }

    /*!
     * @brief Get pat at the given index
     * @param index Index to retrieve the pad at
     * @return Pad
     */
    const Pad& PadAt(int index) const { return mPads[Math::clampi(index, 0, Input::sMaxInputDevices - 1)]; }

    /*!
     * @brief Swap pads at the given positions
     * @param index1 First pad index
     * @param index2 Second pad index
     */
    void Swap(int index1, int index2);

  private:
    //! Pad array
    PadArray mPads;
    //! Connected pad list
    PadList mConnected;
    //! Unconnected pad list
    PadList mUnconnected;

    //! Interface
    IInputChange* mInterface;

    //! Rebuid the pad array, ready to be used
    void Build();

    /*!
     * @brief Load configuration from recalbox.conf
     */
    void LoadConfiguration();

    /*!
     * @brief Save configuration into recalbox.conf
     */
    void SaveConfiguration();

    //! Get available pad list
    static PadList AvailablePads();

    /*!
     * @brief Sort pads, pushing active ones first
     * This sort does not change the order of active pads
     * @param list List to sort
     */
    static void SortActiveFirst(PadArray& padArray);

    /*!
     * @brief Lookup the given pad in the given connected pad list and returns its identifier
     * @param list List to lookup pad
     * @param pad Pad to lookup
     * @return Device identifier or -1 if not found
     */
    static int LookupDevice(const PadList& list, const Pad& pad);

    /*!
     * @brief Lookup the given pad in the given connected pad list
     * If found, the connected device is removed from the connected list
     * and its identifier is returned.
     * @param list List to lookup pad
     * @param pad Pad to lookup
     * @return Device identifier or -1 if not found
     */
    static int LookupAndPopDevice(PadList& list, const Pad& pad);

    /*
     * IInputChange implementation
     */

    //! Refresh pad list
    void PadsAddedOrRemoved() override;
};



