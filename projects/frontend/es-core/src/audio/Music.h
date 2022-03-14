#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <string>
#include <map>
#include <memory>
#include <utils/os/fs/Path.h>
#include <SDL2/SDL_mixer.h>

class ThemeData;

class Music
{
  private:
    //! Current music playing
    static Music*   sCurrentlyPlaying;
    //! Music path
    Path            mPath;
    //! SDL Music
    Mix_Music*      mMusic;

    /*!
     * @brief Load music into SDL structures
     */
    void Initialize();
    /*!
     * @brief Free SDL structures
     */
    void Finalize();

    /*!
     * @brief Direct constructor
     * @param eventConsumer AudioManager instance that consume music events
     * @param path Music filepath
     */
    explicit Music(const Path & path);

  public:
    /*!
     * @brief Destructor
     */
    ~Music();

    /*!
     * @brief Play current music
     * @param repeat True to play the current music in loop
     * @return True if the music started successfuly
     */
  	bool Play(bool repeat);

    static void Pause();
    static void Resume();

    /*!
  	 * @brief Stop current music if it's playing
  	 */
  	static void Stop();

  	/*!
  	 * @brief Return music name
  	 * @return Music name
  	 */
  	std::string Name() const { return mPath.FilenameWithoutExtension(); }

    /*!
     * @brief Return music path
     * @return Music path
     */
    const Path& FilePath() const { return mPath; }

    /*!
  	 * @brief Build a new music file
     * @param path Music filepath
  	 * @return New music instance or null if the file does not exist
  	 */
  	static Music* LoadFromPath(const Path & path);

  	/*!
  	 * @brief Get the music currently playing
  	 * @return Currentply playing music or nullptr
  	 */
  	static Music* CurrentlyPlaying() { return sCurrentlyPlaying; }
};

#endif
