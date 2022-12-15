
#include <sys/socket.h>
#include <utils/os/system/Thread.h>
#include <utils/sync/SyncMessageSender.h>

class SystemManager;
class FileData;

class CommandThread: private Thread
                   , private ISyncMessageReceiver<FileData*>
{
  public:
    /*!
     * @brief Default Constructor
     */
    explicit CommandThread(SystemManager& systemManager);

    /*!
     * @brief Destructor
     */
    ~CommandThread() override;

  private:
    //! Udp port
    static constexpr int sPort = 1337;

    //! SystemManager instance
    SystemManager& mSystemManager;

    //! Socket handle
    int mSocket;
    //! Synchronous event
    SyncMessageSender<FileData*> mEvent;

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
    [[nodiscard]] String ReadUDP() const;

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
     * @brief Receive message from the command thread
     * @param game Game data
     */
    void ReceiveSyncMessage(FileData* const& game) override;
};
