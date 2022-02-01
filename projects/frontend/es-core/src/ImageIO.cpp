#include "ImageIO.h"
#include <utils/Log.h>
#include <FreeImage.h>

std::vector<unsigned char> ImageIO::loadFromMemoryRGBA32(const unsigned char * data, const size_t size, size_t & width, size_t & height)
{
	std::vector<unsigned char> rawData;
	width = 0;
	height = 0;
	FIMEMORY * fiMemory = FreeImage_OpenMemory((BYTE *)data, (DWORD)size);
	if (fiMemory != nullptr)
	{
		//detect the filetype from data
		FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(fiMemory);
		if (format != FIF_UNKNOWN && (FreeImage_FIFSupportsReading(format) != 0))
		{
			//file type is supported. load image
			FIBITMAP* fiBitmap = FreeImage_LoadFromMemory(format, fiMemory);
			if (fiBitmap != nullptr)
			{
				//loaded. convert to 32bit if necessary
				if (FreeImage_GetBPP(fiBitmap) != 32)
				{
					FIBITMAP* fiConverted = FreeImage_ConvertTo32Bits(fiBitmap);
					if (fiConverted != nullptr)
					{
						//free original bitmap data
						FreeImage_Unload(fiBitmap);
						fiBitmap = fiConverted;
					}
				}
        width = FreeImage_GetWidth(fiBitmap);
        height = FreeImage_GetHeight(fiBitmap);
        // loop through scanlines and add all pixel data to the return vector
        // this is necessary, because width*height*bpp might not be == pitch
        // do on-the-fly argb to abgr convertion
        rawData.resize(width * height *4);
        unsigned char* tempData = rawData.data();
        int w = (int)width;
        for (int y = (int)height; --y >= 0; )
        {
          unsigned int* argb = (unsigned int*)FreeImage_GetScanLine(fiBitmap, y);
          unsigned int* abgr = (unsigned int*)(tempData + (y * width * 4));
          for (int x = w ; --x >= 0;)
          {
            unsigned int c = argb[x];
            abgr[x] = (c & 0xFF00FF00) | ((c & 0xFF) << 16) | ((c >> 16) & 0xFF);
          }
        }
        //free bitmap data
        FreeImage_Unload(fiBitmap);
			}
			else { LOG(LogError) << "[Image] Error - Failed to load image from memory!"; }
		}
		else { LOG(LogError) << "[Image] Error - File type " << (format == FIF_UNKNOWN ? "unknown" : "unsupported") << "!"; }
		//free FIMEMORY again
		FreeImage_CloseMemory(fiMemory);
	}
	return rawData;
}

void ImageIO::flipPixelsVert(unsigned char* imagePx, const size_t& width, const size_t& height)
{
	unsigned int* arr = (unsigned int*)imagePx;
  unsigned int* s1 = arr;
  unsigned int* s2 = arr + (height - 1) * width;
	for (int y = (int)height / 2; --y >= 0; s1 += width, s2 -= width)
		for (int x = (int)width; --x >= 0; )
		{
			unsigned int temp = s1[x];
			s1[x] = s2[x];
			s2[x] = temp;
		}
}
