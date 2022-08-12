//
// Created by bkg2k on 25/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <concepts>
class SyncMessageFactory;

class StaticSyncMessageHolder
{
  public:
    //! Fixed message length
    static constexpr int sFixedSize = 64 - sizeof(int);
};

//! Statically check if the type T is less or equal to the max size defined in StaticMessageHolder
template<class T>
constexpr bool IsSizeCompatible = (sizeof(T) <= StaticSyncMessageHolder::sFixedSize);

//! Simple concept of a POD class
template<class T>
concept IsPod = ((std::is_standard_layout<T>::value && std::is_trivial<T>::value  && IsSizeCompatible<T>) || std::is_void<T>::value);

template<IsPod T> union SyncMessageBody
{
  char __padding__[StaticSyncMessageHolder::sFixedSize]; //!< Data placeholder
  T mData;                      //!< User data
};

template<> union SyncMessageBody<void>
{
  char __padding__[StaticSyncMessageHolder::sFixedSize]; //!< Data placeholder
  struct {} mData;
};

/*!
 * @brief Fixed-size user-data containing message
 * @tparam T A POD type so that message of any type T can be exchanged and reused for any other
 */
template<IsPod T> class SyncMessage : StaticSyncMessageHolder
{
  public:
    //! Anonymous union so that mData is straightly accessible
    //! and union'ed with a fixed size array of data
    SyncMessageBody<T> mBody;

  private:
    //! Message identifier, only for routing purpose
    int mIdentifier;

    //! Give access to factory
    friend class SyncMessageFactory;
};

//! Untyped message (containing empty message)
typedef SyncMessage<void> UntypedSyncMessage;