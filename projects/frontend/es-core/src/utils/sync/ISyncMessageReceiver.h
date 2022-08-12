//
// Created by bkg2k on 25/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/sync/SyncMessage.h>

/*!
 * @brief Untyped message receiver
 */
class IUntypedSyncMessageReceiver
{
  public:
    //! Default destructor
    virtual ~IUntypedSyncMessageReceiver() = default;

  protected:
    /*!
     * @brief Receive an untyped message
     * @param message Message object
     */
    virtual void ReceiveUntypedSyncMessage(const UntypedSyncMessage& message) = 0;

    //! Give access to factory
    friend class SyncMessageFactory;
};

/*!
 * @brief Generic typed message receiver
 * @tparam T User message type
 */
template<IsPod T> class ISyncMessageReceiver : public IUntypedSyncMessageReceiver
{
  public:
    /*!
     * @brief Receive a synchronized typed message
     * @param message Message object
     */
    virtual void ReceiveSyncMessage(const T& message) = 0;

  private:
    /*!
     * @brief Receive an untyped message
     * @param message Message object
     */
    void ReceiveUntypedSyncMessage(const UntypedSyncMessage& message) final
    {
      ReceiveSyncMessage(((SyncMessage<T>*)&message)->mBody.mData);
    };
};

/*!
 * @brief Specialized void message receiver
 */
template<> class ISyncMessageReceiver<void> : public IUntypedSyncMessageReceiver
{
  public:
    /*!
     * @brief Receive a synchronized void message
     */
    virtual void ReceiveSyncMessage() = 0;

  private:
    /*!
     * @brief Receive an untyped message
     * @param message Message object
     */
    void ReceiveUntypedSyncMessage(const UntypedSyncMessage& message) final
    {
      (void)message;
      ReceiveSyncMessage();
    };
};

#define DefineSimpleTypeSyncMessageReceiver(typeName) \
/*! \
 * @brief Specialized simple typeName message receiver \
 */ \
template<> class ISyncMessageReceiver<typeName> : public IUntypedSyncMessageReceiver \
{ \
  public: \
    /*! \
     * @brief Receive a synchronized typeName message \
     */ \
    virtual void ReceiveSyncMessage(typeName message) = 0; \
 \
  private: \
    /*! \
     * @brief Receive an untyped message \
     * @param message Message object \
     */ \
    void ReceiveUntypedSyncMessage(const UntypedSyncMessage& message) final \
    { \
      ReceiveSyncMessage(((SyncMessage<typeName>*)&message)->mBody.mData); \
    }; \
};

DefineSimpleTypeSyncMessageReceiver(bool)
DefineSimpleTypeSyncMessageReceiver(char)
DefineSimpleTypeSyncMessageReceiver(unsigned char)
DefineSimpleTypeSyncMessageReceiver(short)
DefineSimpleTypeSyncMessageReceiver(unsigned short)
DefineSimpleTypeSyncMessageReceiver(int)
DefineSimpleTypeSyncMessageReceiver(unsigned int)
DefineSimpleTypeSyncMessageReceiver(long long)
DefineSimpleTypeSyncMessageReceiver(unsigned long long)
DefineSimpleTypeSyncMessageReceiver(float)
DefineSimpleTypeSyncMessageReceiver(double)
