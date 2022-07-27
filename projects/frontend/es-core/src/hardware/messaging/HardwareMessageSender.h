//
// Created by bkg2k on 03/11/2020.
//
#pragma once

#include "IHardwareNotifications.h"
#include "MessageTypes.h"
#include "HardwareMessage.h"
#include <utils/sync/SyncMessageSender.h>

class HardwareMessageSender: private ISyncMessageReceiver<HardwareMessage>
{
  public:
    /*!
     * @brief Constructor
     * @param notificationInterface Notification interface to send messages to
     */
    explicit HardwareMessageSender(IHardwareNotifications& notificationInterface);

    /*!
     * @brief Send a simple message
     * @param board Board type
     * @param message Message type
     */
    void Send(BoardType board, MessageTypes message);

    /*!
     * @brief Send a timed message
     * @param board Board type
     * @param message Message type
     */
    void Send(BoardType board, MessageTypes message, int milliseconds);

  private:
    //! Target interface to send synchronoized message to
    IHardwareNotifications& mNotificationInterface;
    //! Synchronous event sender
    SyncMessageSender<HardwareMessage> mSender;

    /*!
     * @brief Receive a synchronized message
     * @param message Message object
     */
    void ReceiveSyncMessage(const HardwareMessage& message) final;

    /*!
     * @brief Process the hardware message
     * @param message Message to process
     */
    void ProcessMessage(const HardwareMessage& message);
};

