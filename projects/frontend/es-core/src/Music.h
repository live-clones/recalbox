#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <string>
#include <map>
#include <memory>
#include <utils/os/fs/Path.h>
#include "SDL_mixer.h"

class ThemeData;

class Music
{
  private:
    //! Music path
    Path            mPath;
    //! SDL Music
    Mix_Music*      mMusic;
    //! Play status
    bool            mIsPlaying;

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

  	/*!
  	 * @brief Stop current music if it's playing
  	 */
  	static void Stop();

  	/*!
  	 * @brief Return music name
  	 * @return Music name
  	 */
  	std::string Name() { return mPath.FilenameWithoutExtension(); }

  	/*!
  	 * @brief Build a new music file
     * @param path Music filepath
  	 * @return New music instance or null if the file does not exist
  	 */
  	static Music* BuildFromPath(const Path & path);
    /*!
     * @brief Build a music from theme elements
     * @param theme ThemeData structure
     * @param view Theme view
     * @param element Theme element
  	 * @return New music instance or null if the theme element/file does not exist
     */
  	static Music* BuildFromTheme(const ThemeData& theme, const std::string& view, const std::string& element);
};

#endif
