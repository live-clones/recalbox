#include "scrapers/Scraper.h"
#include "utils/Log.h"
#include "Settings.h"
#include <FreeImage.h>
#include <RootFolders.h>
#include <utils/Files.h>

#include "GamesDBJSONScraper.h"
#include "ScreenscraperScraper.h"

#define SCRAPER_THEGAMEDB     "TheGamesDB"
#define SCRAPER_SCREENSCRAPER "Screenscraper"

static const std::map<std::string, generate_scraper_requests_func>& getScraperGeneratorList()
{
  static const std::map<std::string, generate_scraper_requests_func> scraperRequestFunction =
  {
    {SCRAPER_THEGAMEDB,     &thegamesdb_generate_json_scraper_requests},
    {SCRAPER_SCREENSCRAPER, &screenscraper_generate_scraper_requests},
  };
  return scraperRequestFunction;
}

std::unique_ptr<ScraperSearchHandle> startScraperSearch(const ScraperSearchParams& params)
{
  // Get prefered scrapper
	std::string name = Settings::getInstance()->getString("Scraper");

	// Control invalid scraper
	if (getScraperGeneratorList().find(name) == getScraperGeneratorList().end())
	  name = SCRAPER_SCREENSCRAPER;

	// Create the scraper
	std::unique_ptr<ScraperSearchHandle> handle(new ScraperSearchHandle());
  getScraperGeneratorList().at(name)(params, handle->mRequestQueue, handle->mResults);

	return handle;
}

std::vector<std::string> getScraperList()
{
	std::vector<std::string> list;
	for (const auto& scraper_request_func : getScraperGeneratorList())
	{
		list.push_back(scraper_request_func.first);
	}

	return list;
}

// ScraperSearchHandle
ScraperSearchHandle::ScraperSearchHandle()
{
	setStatus(AsyncStatus::InProgress);
}

void ScraperSearchHandle::update()
{
	if(mStatus == AsyncStatus::Done)
		return;

	while(!mRequestQueue.empty())
	{
		auto& req = mRequestQueue.front();
    AsyncStatus status = req->status();

		if(status == AsyncStatus::Error)
		{
			// propegate error
			setError(req->getStatusString());

			// empty our queue
			while(!mRequestQueue.empty())
				mRequestQueue.pop();

			return;
		}

		// finished this one, see if we have any more
		if(status == AsyncStatus::Done)
		{
			mRequestQueue.pop();
			continue;
		}

		// status == ASYNC_IN_PROGRESS
	}

	// we finished without any errors!
	if(mRequestQueue.empty())
	{
		setStatus(AsyncStatus::Done);
		return;
	}
}



// ScraperRequest
ScraperRequest::ScraperRequest(std::vector<ScraperSearchResult>& resultsWrite)
  : mResults(resultsWrite)
{
}


// ScraperHttpRequest
ScraperHttpRequest::ScraperHttpRequest(std::vector<ScraperSearchResult>& resultsWrite, const std::string& url) 
	: ScraperRequest(resultsWrite)
{
	setStatus(AsyncStatus::InProgress);
	mReq = std::unique_ptr<HttpReq>(new HttpReq(url));
}

void ScraperHttpRequest::update()
{
	HttpReq::Status status = mReq->status();
	if(status == HttpReq::Status::Success)
	{
		setStatus(AsyncStatus::Done); // if process() has an error, status will be changed to ASYNC_ERROR
		process(mReq, mResults);
		return;
	}

	// not ready yet
	if(status == HttpReq::Status::InProgress)
		return;

	// everything else is some sort of error
	LOG(LogError) << "ScraperHttpRequest network error (status: " << (int)status << ") - " << mReq->getErrorMsg();
	setError(mReq->getErrorMsg());
}


// metadata resolving stuff

std::unique_ptr<MDResolveHandle> resolveMetaDataAssets(const ScraperSearchResult& result, const ScraperSearchParams& search)
{
	return std::unique_ptr<MDResolveHandle>(new MDResolveHandle(result, search));
}

MDResolveHandle::MDResolveHandle(const ScraperSearchResult& result, const ScraperSearchParams& search) : mResult(result)
{
	if(!result.imageUrl.empty())
	{
		Path imgPath = getSaveAsPath(search, "images", result.imageType);
		mFuncs.push_back(ResolvePair(downloadImageAsync(result.imageUrl, imgPath), [this, imgPath]
		{
			mResult.mdl.SetImagePath(imgPath.ToString());
			mResult.imageUrl.clear();
		}));
	}
}

void MDResolveHandle::update()
{
	if(mStatus == AsyncStatus::Done || mStatus == AsyncStatus::Error)
		return;
	
	auto it = mFuncs.begin();
	while(it != mFuncs.end())
	{
		if(it->first->status() == AsyncStatus::Error)
		{
			setError(it->first->getStatusString());
			return;
		}else if(it->first->status() == AsyncStatus::Done)
		{
			it->second();
			it = mFuncs.erase(it);
			continue;
		}
		it++;
	}

	if(mFuncs.empty())
		setStatus(AsyncStatus::Done);
}

std::unique_ptr<ImageDownloadHandle> downloadImageAsync(const std::string& url, const Path& saveAs)
{
	return std::unique_ptr<ImageDownloadHandle>(new ImageDownloadHandle(url, saveAs, 
		Settings::getInstance()->getInt("ScraperResizeWidth"), Settings::getInstance()->getInt("ScraperResizeHeight")));
}

ImageDownloadHandle::ImageDownloadHandle(const std::string& url, const Path& path, int maxWidth, int maxHeight)
  : mReq(new HttpReq(url)),
	  mSavePath(path),
	  mMaxWidth(maxWidth),
	  mMaxHeight(maxHeight)
{
}

void ImageDownloadHandle::update()
{
	if(mReq->status() == HttpReq::Status::InProgress)
		return;

	if(mReq->status() != HttpReq::Status::Success)
	{
		std::stringstream ss;
		ss << "Network error: " << mReq->getErrorMsg();
		setError(ss.str());
		return;
	}

  if (mReq->getContent().size() > 256) // handle the "NOMEDIA" text reponse :(
  {
    // Save
    if (!Files::SaveFile(mSavePath, mReq->getContent()))
    {
      setError("Failed to save image. Disk full?");
      return;
    }
    // Resize
    if (!resizeImage(mSavePath, mMaxWidth, mMaxHeight))
    {
      setError("Error saving resized image. Out of memory? Disk full?");
      return;
    }
  }

	setStatus(AsyncStatus::Done);
}

//you can pass 0 for width or height to keep aspect ratio
bool resizeImage(const Path& path, int maxWidth, int maxHeight)
{
	// nothing to do
	if(maxWidth == 0 && maxHeight == 0)
		return true;

	FREE_IMAGE_FORMAT format = FIF_UNKNOWN;
	FIBITMAP* image = nullptr;
	
	//detect the filetype
	format = FreeImage_GetFileType(path.ToChars(), 0);
	if(format == FIF_UNKNOWN)
		format = FreeImage_GetFIFFromFilename(path.ToChars());
	if(format == FIF_UNKNOWN)
	{
		LOG(LogError) << "Error - could not detect filetype for image \"" << path.ToString() << "\"!";
		return false;
	}

	//make sure we can read this filetype first, then load it
	if(FreeImage_FIFSupportsReading(format) != 0)
	{
		image = FreeImage_Load(format, path.ToChars());
	}else{
		LOG(LogError) << "Error - file format reading not supported for image \"" << path.ToString() << "\"!";
		return false;
	}

	unsigned int width = FreeImage_GetWidth(image);
	unsigned int height = FreeImage_GetHeight(image);

	if(maxWidth == 0)
	{
		maxWidth = (int)(((double)maxHeight / height) * width);
	}else if(maxHeight == 0)
	{
		maxHeight = (int)(((double)maxWidth / width) * height);
	}

	FIBITMAP* imageRescaled = FreeImage_Rescale(image, maxWidth, maxHeight, FILTER_BILINEAR);
	FreeImage_Unload(image);

	if(imageRescaled == nullptr)
	{
		LOG(LogError) << "Could not resize image! (not enough memory? invalid bitdepth?)";
		return false;
	}

	bool saved =  path.Empty() ? false : (FreeImage_Save(format, imageRescaled, path.ToChars()) != 0);
	FreeImage_Unload(imageRescaled);

    if(!saved) {
		LOG(LogError) << "Failed to save resized image!";
    }

	return saved;
}

Path getSaveAsPath(const ScraperSearchParams& params, const std::string& suffix, const ScraperImageType type)
{
	const Path rootFolder = params.system->getRootFolder()->getPath();
  const Path gameFolder = params.game->getPath().Directory();
	const std::string gameName = params.game->getPath().Filename();

	bool dummy;
	Path subPath = gameFolder.MakeRelative(rootFolder, dummy);

	// default dir in rom directory
	Path path = params.system->getRootFolder()->getPath() / "media" / suffix;
	if (!subPath.Empty()) path = path / subPath;
	if (!path.Exists())
  {
	  path.CreatePath();
    if (!path.Exists())
    {
      LOG(LogError) << "Cannot create " << path.ToString();
      return Path();
    }
  }

	std::string gameExt = ".png"; // Default
	switch(type)
  {
    case ScraperImageType::Jpeg: gameExt = ".jpg";
    case ScraperImageType::Png:
    default: break;
  }

	path = path / gameName;
	return path.ChangeExtension(gameExt);
}
