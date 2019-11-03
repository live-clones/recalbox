#include "Music.h"
#include "utils/Log.h"
#include "Settings.h"
#include "themes/ThemeData.h"
#include "AudioManager.h"

std::map<Path, std::shared_ptr<Music>> Music::sMap;

std::shared_ptr<Music> Music::get(const Path& path)
{
	auto it = sMap.find(path);
	if(it != sMap.end())
		return it->second;
	std::shared_ptr<Music> music = std::shared_ptr<Music>(new Music(path));
	sMap[path] = music;
        AudioManager::getInstance()->registerMusic(music);

	return music;
}

std::shared_ptr<Music> Music::getFromTheme(const ThemeData& theme, const std::string& view, const std::string& element)
{
	LOG(LogInfo) << " req music [" << view << "." << element << "]";
	const ThemeData::ThemeElement* elem = theme.getElement(view, element, "sound");
	if((elem == nullptr) || !elem->has("path"))
	{
		LOG(LogInfo) << "   (missing)";
		return nullptr;
	}
	return get(Path(elem->get<std::string>("path")));
}

Music::Music(const Path & path)
  : mPath(path),
    music(nullptr),
    playing(false)
{
	initMusic();
}

Music::~Music()
{
	deinitMusic();
}

std::string Music::getName()
{
  return mPath.FilenameWithoutExtension();
}

void Music::initMusic()
{
	if(music != nullptr)
		deinitMusic();

	if(mPath.Empty())
		return;

	//load wav file via SDL
        Mix_Music *gMusic = nullptr;
        gMusic = Mix_LoadMUS( mPath.ToChars() );
        if(gMusic == nullptr){
            LOG(LogError) << "Error loading sound \"" << mPath.ToString() << "\"!\n" << "	" << SDL_GetError();
            return;
        }else {
            music = gMusic;
        }
}

void Music::deinitMusic()
{
	playing = false;
        if(music != nullptr){
            Mix_FreeMusic( music );
            music = nullptr;
        }
}

void Music::play(bool repeat, void (* callback)())
{
    if(music == nullptr)
		return;
	if (!playing)
	{
		playing = true;
	}
    LOG(LogInfo) << "playing";
    if(Mix_FadeInMusic(music, repeat ? -1 : 1, 1000) == -1){
        LOG(LogInfo) << "Mix_PlayMusic: " << Mix_GetError();
		return;
    }
	if(!repeat){
		Mix_HookMusicFinished(callback);
	}
}