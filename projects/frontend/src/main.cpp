#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "Renderer.h"
#include "components/GuiGameList.h"
#include "SystemData.h"
#include <boost/filesystem.hpp>
#include "components/GuiInputConfig.h"

int main(int argc, char* argv[])
{
	bool running = true;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
	{
		std::cerr << "Error - could not initialize SDL!\n";
		std::cerr << "	" << SDL_GetError() << "\n";
		std::cerr << "\nAre you in the 'video' and 'input' groups?\n";
		return 1;
	}
	if(TTF_Init() != 0)
	{
		std::cerr << "Error - could not initialize SDL_ttf!\n";
		std::cerr << "	" << TTF_GetError() << "\n";
		return 1;
	}


	int width = 0;
	int height = 0;
	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-w") == 0)
			{
				width = atoi(argv[i + 1]);
				i++;
			}else if(strcmp(argv[i], "-h") == 0)
			{
				height = atoi(argv[i + 1]);
				i++;
			}
		}
	}

	Renderer::screen = SDL_SetVideoMode(width, height, 16, SDL_SWSURFACE | SDL_FULLSCREEN);

	if(Renderer::screen == NULL)
	{
		std::cerr << "Error - could not set video mode!\n";
		std::cerr << "	" << SDL_GetError() << "\n";
		std::cerr << "\nYou may want to try using -w and -h to specify a resolution.\n";
		return 1;
	}else{
		std::cout << "Video mode is " << Renderer::screen->w << "x" << Renderer::screen->h << "\n";
	}

	SDL_ShowCursor(false);
	SDL_EnableKeyRepeat(500, 100);
	SDL_JoystickEventState(SDL_ENABLE);

	if(!boost::filesystem::exists(SystemData::getConfigPath()))
	{
		std::cerr << "A system config file in $HOME/.es_systems.cfg was not found. An example will be created.\n";
		SystemData::writeExampleConfig();
		std::cerr << "Set it up, then re-run EmulationStation.\n";
		running = false;
	}else{
		SystemData::loadConfig();

		if(SystemData::sSystemVector.size() == 0)
		{
			std::cerr << "A system config file in $HOME/.es_systems.cfg was found, but contained no systems.\n";
			std::cerr << "You should probably go read that, or delete it.\n";
			running = false;
		}else{
			if(boost::filesystem::exists(InputManager::getConfigPath()))
			{
				InputManager::loadConfig();
				new GuiGameList();
			}else{
				if(SDL_NumJoysticks() > 0)
				{
					new GuiInputConfig();
				}else{
					std::cout << "Note - it looks like you have no joysticks connected.\n";
					new GuiGameList();
				}
			}
		}
	}


	while(running)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_JOYHATMOTION:
				case SDL_JOYAXISMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
				case SDL_KEYDOWN:
					InputManager::processEvent(&event);
					break;
				case SDL_KEYUP:
					InputManager::processEvent(&event);
					break;

				case SDL_QUIT:
					running = false;
					break;
			}
		}

		Renderer::render();
		SDL_Flip(Renderer::screen);
	}

	Renderer::deleteAll();
	SystemData::deleteSystems();

	std::cout << "EmulationStation cleanly shutting down...\n";

	TTF_Quit();
	SDL_Quit();
	return 0;
}
