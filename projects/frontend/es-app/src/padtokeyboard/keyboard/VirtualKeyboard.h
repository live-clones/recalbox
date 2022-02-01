//
// Created by bkg2k on 21/12/2019.
//
#pragma once

class VirtualKeyboard
{
  public:
    //! Max event in list
    static constexpr int sMax = 32;
    //! Compact event
    struct Event
    {
      short Key;     //!< Key code
      bool  Pressed; //!< True = pressed, False = released
    } __attribute__((packed));

    //! Compact event list
    struct EventList
    {
      int Count;
      int Delay;
      Event Events[32];
    };

    /*!
     * @brief Constructor
     */
    VirtualKeyboard();

    /*!
     * @brief Destructor
     */
    ~VirtualKeyboard();

    /*!
     * @brief Open the virtual keyboard /dev file
     */
    void Open();

    /*!
     * @brief Write an event (key press/release) to the virtual kayboard
     * @param event Key and state
     */
    void Write(const Event& event);

    /*!
     * @brief Check virtual keyboard readiness
     * @return True if the virtual keyboard is ready
     */
    bool Ready() const { return (mFileDescriptor >= 0); }

  private:
    //! Virtual keyboard file descriptor
    int mFileDescriptor;

    /*!
     * @brief Close virtual keyboard
     */
    void Close() const;

    /*!
     * @brief Emit low level event into the virtual keyboard
     * @param type Event type
     * @param code Event code
     * @param value Event value
     */
    void Emit(int type, int code, int value) const;
};

