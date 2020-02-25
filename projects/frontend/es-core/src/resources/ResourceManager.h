#pragma once

#include <memory>
#include <utils/os/fs/Path.h>
#include <resources/IReloadable.h>

//The ResourceManager exists to...
//Allow loading resources embedded into the executable like an actual file.
//Allow embedded resources to be optionally remapped to actual files for further customization.

//!< Use a simple string as byte buffer
typedef std::string ResourceData;

class ResourceManager
{
public:
	static ResourceManager* getInstance();

	void addReloadable(const std::weak_ptr<IReloadable>& reloadable);

	void unloadAll();
	void reloadAll();

	static ResourceData getFileData(const Path& path) ;
	static bool fileExists(const Path& path) ;

private:
	ResourceManager() = default;

	static ResourceManager* sInstance;

	static ResourceData loadFile(const Path& path) ;

	std::vector< std::weak_ptr<IReloadable> > mReloadables;
};
