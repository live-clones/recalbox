#pragma once

#include <memory>
#include <list>
#include <utils/os/fs/Path.h>

//The ResourceManager exists to...
//Allow loading resources embedded into the executable like an actual file.
//Allow embedded resources to be optionally remapped to actual files for further customization.

//!< Use a simple string as byte buffer
typedef std::string ResourceData;

class ResourceManager;

class IReloadable
{
public:
	virtual void unload(std::shared_ptr<ResourceManager>& rm) = 0;
	virtual void reload(std::shared_ptr<ResourceManager>& rm) = 0;
};

class ResourceManager
{
public:
	static std::shared_ptr<ResourceManager>& getInstance();

	void addReloadable(const std::weak_ptr<IReloadable>& reloadable);

	void unloadAll();
	void reloadAll();

	ResourceData getFileData(const Path& path) const;
	bool fileExists(const Path& path) const;

private:
	ResourceManager() = default;

	static std::shared_ptr<ResourceManager> sInstance;

	ResourceData loadFile(const Path& path) const;

	std::list< std::weak_ptr<IReloadable> > mReloadables;
};
