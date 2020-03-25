//
// Created by Bkg2k on 11/03/2020.
//
#pragma once

class TCPNetwork
{
  private:
    //! Socket
    int mSocket;

  public:
    /*!
     * @brief Default constructor
     */
    TCPNetwork()
      : mSocket(0)
    {
    }

    /*!
     * @brief
     * @param hostname MQTT hostame - nullptr means local loopback 127.0.0.1
     * @param port MQTT Server port - 0 means default MQTT port 1883
     * @return 0 if no error, else errno value
     */
    int connect(const char* hostname, int port);

    /*!
     * @brief Read byte on socket
     * @param buffer Buffer to fill in with read bytes
     * @param len Buffer size
     * @param timeout_ms Timeout in milliseconds
     * @return -1 on error, 0 on timeout, or number of byte read
     */
    int read(unsigned char* buffer, int len, int timeout_ms);

    /*!
     * @brief Write byte on socket
     * @param buffer Buffer to write
     * @param len Buffer length
     * @param timeout Timeout in milliseconds
     * @return -1 on error, 0 on timeout, or number of byte read
     */
    int write(unsigned char* buffer, int len, int timeout);

    /*!
     * @brief Close connection
     * @return 0 if no error, else errno value
     */
    int disconnect();

    /*!
     * @brief Check if the socket is connected
     * @return True if the socket is connected, false otherwise
     */
    bool isConnected() const { return mSocket != 0; }
};
