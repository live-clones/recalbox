#include <utils/Files.h>
#include "ResourceManager.h"
#include "../data/Resources.h"

ResourceManager* ResourceManager::sInstance = nullptr;

ResourceManager* ResourceManager::getInstance()
{
	if (sInstance == nullptr)
		sInstance = new ResourceManager();

	return sInstance;
}

ResourceData ResourceManager::getFileData(const Path& path)
{
	//check if its embedded
	if(res2hMap.find(path.ToString()) != res2hMap.end())
	{
		//it is
		Res2hEntry embeddedEntry = res2hMap.find(path.ToString())->second;
		ResourceData data((const char*)embeddedEntry.data, embeddedEntry.size);
		return data;
	}

	//it's not embedded; load the file
	if(!path.Exists())
	{
		//if the file doesn't exist, return an "empty" ResourceData
		return ResourceData();
	}else{
		ResourceData data = loadFile(path);
		return data;
	}
}

ResourceData ResourceManager::loadFile(const Path& path)
{
  return Files::LoadFile(path);
}

bool ResourceManager::fileExists(const Path& path)
{
	//if it exists as an embedded file, return true
	if(res2hMap.find(path.ToString()) != res2hMap.end())
		return true;

	return path.Exists();
}

void ResourceManager::unloadAll()
{
	auto iter = mReloadables.begin();
	while(iter != mReloadables.end())
	{
		if(!iter->expired())
		{
			iter->lock()->unload(*sInstance);
			iter++;
		}else{
			iter = mReloadables.erase(iter);
		}
	}
}

void ResourceManager::reloadAll()
{
	auto iter = mReloadables.begin();
	while(iter != mReloadables.end())
	{
		if(!iter->expired())
		{
			iter->lock()->reload(*sInstance);
			iter++;
		}else{
			iter = mReloadables.erase(iter);
		}
	}
}

void ResourceManager::addReloadable(const std::weak_ptr<IReloadable>& reloadable)
{
	mReloadables.push_back(reloadable);
}
