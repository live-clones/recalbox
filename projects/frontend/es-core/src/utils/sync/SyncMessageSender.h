//
// Created by bkg2k on 25/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/sync/ISyncMessageReceiver.h>
#include <utils/sync/SyncMessage.h>
#include <utils/sync/SyncMessageFactory.h>

/*!
 * @brief Base sender
 * @tparam T User message type
 */
class SyncMessageSenderBase
{
  public:
    /*!
     * @brief Default constructor
     * @param receiver Receiver that will receive all message sent through this sender
     */
    explicit SyncMessageSenderBase(IUntypedSyncMessageReceiver& receiver)
    : mFactory(SyncMessageFactory::Instance())
    , mMessageIdentifier(mFactory.Register(receiver))
    {
    }

    ~SyncMessageSenderBase()
    {
      mFactory.Unregister(mMessageIdentifier);
    }

    //! Factory direct access
    SyncMessageFactory& mFactory;
    //! Message identifier
    int mMessageIdentifier;
};

/*!
 * @brief Generic sender
 * @tparam T User message type
 */
template<IsPod T> class SyncMessageSender : public SyncMessageSenderBase
{
  public:
    explicit SyncMessageSender(ISyncMessageReceiver<T>& receiver)
      : SyncMessageSenderBase(receiver)
    {
    }

    /*!
     * @brief Send a message
     * @param message Typed message
     */
    void Send(const T& userMessage)
    {
      SyncMessage<T>* message = (SyncMessage<T>*)mFactory.Obtain();
      message->mBody.mData = userMessage;
      mFactory.Push((UntypedSyncMessage*)message, mMessageIdentifier);
    }
};

/*!
 * @brief Specialized void sender
 */
template<> class SyncMessageSender<void> : public SyncMessageSenderBase
{
  public:
    explicit SyncMessageSender(ISyncMessageReceiver<void>& receiver)
      : SyncMessageSenderBase(receiver)
    {
    }

    /*!
     * @brief Send a message
     * @param message Typed message
     */
    void Send()
    {
      SyncMessage<void>* message = (SyncMessage<void>*)mFactory.Obtain();
      mFactory.Push((UntypedSyncMessage*)message, mMessageIdentifier);
    }
};