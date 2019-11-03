#include <utils/os/system/Thread.h>
#include <utils/sdl2/ISyncronousEvent.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <sys/socket.h>

class CommandThread: private Thread, private ISyncronousEvent
{
  public:
    /*!
     * @brief Default Constructor
     */
    explicit CommandThread();

    /*!
     * @brief Destructor
     */
    ~CommandThread() override;

  private:
    //! Udp port
    static constexpr int sPort = 1337;

    //! Socket handle
    int mSocket;
    //! Synchronous event
    SyncronousEvent mEvent;

    /*!
     * @brief Try to open the socket continuously.
     * Returns true if the socket has been opened successfully.
     * Returns false if the thread is closed before the socket is opened
     * @return true if the socket is opened
     */
    bool OpenUDP();

    /*!
     * @brief Read an UDP packets and convert the buffer to a string
     * @return Received string
     */
    std::string ReadUDP();

    /*
     * Thread overrides
     */

    /*!
     * @brief Main thread routine
     */
    void Run() override;
    /*!
     * @brief Close the socket when stop is called
     */
    void Break() override
    {
      shutdown(mSocket, SHUT_RDWR);
      close(mSocket);
    }

    /*
     * Synchronous event
     */

    /*!
     * @brief Receive SDL event from the main thread
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;
};
