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
  Path mPath;
	Mix_Music* music;
	bool playing;

public:
        
	static std::shared_ptr<Music> getFromTheme(const ThemeData& theme, const std::string& view, const std::string& element);
	static std::shared_ptr<Music> get(const Path& path);
	void play(bool repeat, void (* callback)());
	std::string getName();
        
	~Music();

private:
	explicit Music(const Path & path);
	static std::map<Path, std::shared_ptr<Music>> sMap;

	void initMusic();
	void deinitMusic();
};

#endif
