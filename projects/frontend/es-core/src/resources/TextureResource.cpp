#include "resources/TextureResource.h"

#include <memory>
#include "hardware/Board.h"
#include "platform_gl.h"
#include "ImageIO.h"

TextureDataManager		TextureResource::sTextureDataManager;
std::map< TextureResource::TextureKeyType, std::weak_ptr<TextureResource> > TextureResource::sTextureMap;
std::set<TextureResource*> 	TextureResource::sAllTextures;

TextureResource::TextureResource(const Path& path, bool tile, bool dynamic)
  : mTextureData(nullptr),
    mSize(0),
    mSourceSize(0.0f),
    mForceLoad(false)
{
// Create a texture data object for this texture
	if (!path.IsEmpty())
	{
		// If there is a path then the 'dynamic' flag tells us whether to use the texture
		// data manager to manage loading/unloading of this texture
		std::shared_ptr<TextureData> data;
		if (dynamic)
		{
			data = sTextureDataManager.add(this, tile);
			data->initFromPath(path);
			// Force the texture manager to load it using a blocking load
			sTextureDataManager.load(data, true);
		}
		else
		{
			mTextureData = std::make_shared<TextureData>(tile);
			data = mTextureData;
			data->initFromPath(path);
			// Load it so we can read the width/height
			data->load();
		}

		mSize.Set(data->width(), data->height());
		mSourceSize.Set(data->sourceWidth(), data->sourceHeight());
	}
	else
	{
		// Create a texture managed by this class because it cannot be dynamically loaded and unloaded
		mTextureData = std::make_shared<TextureData>(tile);
	}
	sAllTextures.insert(this);
}

TextureResource::~TextureResource()
{
	if (mTextureData == nullptr)
		sTextureDataManager.remove(this);
	sAllTextures.erase(sAllTextures.find(this));
}

void TextureResource::initFromPixels(const unsigned char* dataRGBA, size_t width, size_t height)
{
	// This is only valid if we have a local texture data object
	assert(mTextureData != nullptr);
	mTextureData->releaseVRAM();
	mTextureData->releaseRAM();
	mTextureData->initFromRGBA(dataRGBA, width, height);
	// Cache the image dimensions
	mSize.Set(width, height);
	mSourceSize.Set(mTextureData->sourceWidth(), mTextureData->sourceHeight());
}

void TextureResource::initFromMemory(const char* data, size_t length)
{
	// This is only valid if we have a local texture data object
	assert(mTextureData != nullptr);
	mTextureData->releaseVRAM();
	mTextureData->releaseRAM();
	mTextureData->initImageFromMemory((const unsigned char*)data, length);
	// Get the size from the texture data
	mSize.Set(mTextureData->width(), mTextureData->height());
	mSourceSize.Set(mTextureData->sourceWidth(), mTextureData->sourceHeight());
}

bool TextureResource::isTiled() const
{
	if (mTextureData != nullptr)
		return mTextureData->tiled();
	std::shared_ptr<TextureData> data = sTextureDataManager.get(this);
	return data->tiled();
}

bool TextureResource::bind()
{
	if (mTextureData != nullptr)
	{
		mTextureData->uploadAndBind();
		return true;
	}
	else
	{
		return sTextureDataManager.bind(this);
	}
}


std::shared_ptr<TextureResource> TextureResource::get(const Path& path, bool tile, bool forceLoad, bool dynamic)
{
	ResourceManager* rm = ResourceManager::getInstance();


	if(path.IsEmpty())
	{
		std::shared_ptr<TextureResource> tex(new TextureResource(Path::Empty, tile, false));
		rm->addReloadable(tex); //make sure we get properly deinitialized even though we do nothing on reinitialization
		return tex;
	}

	TextureKeyType key(path, tile);
	auto foundTexture = sTextureMap.find(key);
	if(foundTexture != sTextureMap.end())
	{
        if(!foundTexture->second.expired()) {
			return foundTexture->second.lock();
        }
	}

	// need to create it
	std::shared_ptr<TextureResource> tex;
	tex = std::shared_ptr<TextureResource>(new TextureResource(Path(key.first), tile, dynamic));
	std::shared_ptr<TextureData> data = sTextureDataManager.get(tex.get());

	// is it an SVG?
	if(key.first.Extension() != ".svg")
	{
		// Probably not. Add it to our map. We don't add SVGs because 2 svgs might be rasterized at different sizes
		sTextureMap[key] = std::weak_ptr<TextureResource>(tex);
	}
	// Add it to the reloadable list
	rm->addReloadable(tex);

	// Force load it if necessary. Note that it may get dumped from VRAM if we run low
	if (forceLoad)
	{
		tex->mForceLoad = forceLoad;
		data->load();
	}

	return tex;
}

// For scalable source images in textures we want to set the resolution to rasterize at
void TextureResource::rasterizeAt(size_t width, size_t height)
{
	std::shared_ptr<TextureData> data;
	if (mTextureData != nullptr)
		data = mTextureData;
	else
		data = sTextureDataManager.get(this);
	mSourceSize.Set((float)width, (float)height);
	data->setSourceSize((float)width, (float)height);
	if (mForceLoad || (mTextureData != nullptr))
		data->load();
}

size_t TextureResource::getTotalMemUsage()
{
	size_t total = 0;

	// Count up all textures that manage their own texture data
	for (auto tex : sAllTextures)
	{
		if (tex->mTextureData != nullptr)
			total += tex->mTextureData->getVRAMUsage();
	}
	// Now get the committed memory from the manager
	total += sTextureDataManager.getCommittedSize();
	// And the size of the loading queue
	total += sTextureDataManager.getQueueSize();
	return total;
}

size_t TextureResource::getTotalTextureSize()
{
	size_t total = 0;
	// Count up all textures that manage their own texture data
	for (auto tex : sAllTextures)
	{
		if (tex->mTextureData != nullptr)
			total += tex->getSize().x() * tex->getSize().y() * 4;
	}
	// Now get the total memory from the manager
	total += sTextureDataManager.getTotalSize();
	return total;
}

void TextureResource::unload(ResourceManager& rm)
{
	(void)rm;

	// Release the texture's resources
	std::shared_ptr<TextureData> data;
	if (mTextureData == nullptr)
		data = sTextureDataManager.get(this);
	else
		data = mTextureData;

	data->releaseVRAM();
	data->releaseRAM();
}

void TextureResource::reload(ResourceManager& rm)
{
	(void)rm;

	// For dynamically loaded textures the texture manager will load them on demand.
	// For manually loaded textures we have to reload them here
	if (mTextureData)
		mTextureData->load();
}
