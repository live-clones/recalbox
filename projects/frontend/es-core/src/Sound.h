#ifndef _SOUND_H_
#define _SOUND_H_

#include <string>
#include <map>
#include <memory>
#include "SDL_mixer.h"
#include <utils/os/fs/Path.h>

class ThemeData;

class Sound
{
  private:
    //! File path
    Path mPath;
    //! SDL sound data
    Mix_Chunk* mSampleData;

    /*!
     * @brief Constructor
     * @param path Path to sound file
     */
    explicit Sound(const Path& path);

    /*!
     * @brief Load sound into SDL structures
     */
    void Initialize();

    /*!
     * @brief Free SDL structures
     */
    void Finalize();

  public:
    /*!
     * @brief Build a new sound file
     * @param path Sound filepath
     * @return New sound instance or null if the file does not exist
     */
    static Sound* BuildFromPath(const Path & path);

    /*!
     * @brief Build a sound from theme elements
     * @param theme ThemeData structure
     * @param view Theme view
     * @param element Theme element
  	 * @return New music instance or null if the theme element/file does not exist
     */
    static Sound* BuildFromTheme(const ThemeData& theme, const std::string& view, const std::string& element);

    /*!
     * @brief Destructor
     */
    ~Sound();

    /*!
     * @brief Play current sound once
     */
    void Play();

    /*!
     * @brief Stop current sound if it's playing
     */
    static void Stop();

};

#endif
