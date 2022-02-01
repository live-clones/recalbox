#pragma once

#include "components/base/Component.h"
#include "components/IList.h"
#include "components/ImageComponent.h"
#include "utils/Log.h"

struct ImageGridData
{
	std::shared_ptr<TextureResource> texture;
};

template<typename T>
class ImageGridComponent : public IList<ImageGridData, T>
{
protected:
	using IList<ImageGridData, T>::mEntries;
	using IList<ImageGridData, T>::listUpdate;
	using IList<ImageGridData, T>::listInput;
	using IList<ImageGridData, T>::listRenderTitleOverlay;
	using IList<ImageGridData, T>::getTransform;
	using IList<ImageGridData, T>::mSize;
	using IList<ImageGridData, T>::mCursor;
    using typename IList<ImageGridData, T>::Entry;
	using IList<ImageGridData, T>::mWindow;

public:
	using IList<ImageGridData, T>::size;
	using IList<ImageGridData, T>::isScrolling;
	using IList<ImageGridData, T>::stopScrolling;

	explicit ImageGridComponent(WindowManager& window);

	void add(const std::string& name, const Path& imagePath, const T& obj);
	
	void onSizeChanged() override;

	bool ProcessInput(const InputCompactEvent& event) override;
	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;

private:
	Vector2f getSquareSize(const std::shared_ptr<TextureResource>& tex = nullptr) const
	{
		Vector2f aspect(1, 1);

		if(tex)
		{
			const Vector2i& texSize = tex->getSize();

			if(texSize.x() > texSize.y())
				aspect[0] = (float)texSize.x() / (float)texSize.y();
			else
				aspect[1] = (float)texSize.y() / (float)texSize.x();
		}

		return {156 * aspect.x(), 156 * aspect.y() };
	};

	Vector2f getMaxSquareSize() const
	{
		Vector2f squareSize(32, 32);

		// calc biggest square size
		for (auto& entry : mEntries)
		{
			Vector2f chkSize = getSquareSize(entry.data.texture);
			if(chkSize.x() > squareSize.x())
				squareSize[0] = chkSize[0];
			if(chkSize.y() > squareSize.y())
				squareSize[1] = chkSize[1];
		}

		return squareSize;
	};

	Vector2i getGridSize() const
	{
		Vector2f squareSize = getMaxSquareSize();
		Vector2i gridSize(mSize.x() / (squareSize.x() + getPadding().x()), mSize.y() / (squareSize.y() + getPadding().y()));
		return gridSize;
	};

	Vector2f getPadding() const { return {24, 24 }; }
	
	void buildImages();
	void updateImages();

	virtual void onCursorChanged(const CursorState& state);

	bool mEntriesDirty;

	std::vector<ImageComponent> mImages;
};

template<typename T>
ImageGridComponent<T>::ImageGridComponent(WindowManager& window)
  : IList<ImageGridData, T>(window)
{
	mEntriesDirty = true;
}

template<typename T>
void ImageGridComponent<T>::add(const std::string& name, const Path& imagePath, const T& obj)
{
	typename IList<ImageGridData, T>::Entry entry;
	entry.name = name;
	entry.object = obj;
	entry.data.texture = ResourceManager::getInstance()->fileExists(imagePath) ? TextureResource::get(imagePath) : TextureResource::get(Path(":/button.png"));
  ((IList< ImageGridData, T >*)this)->add(entry);
	mEntriesDirty = true;
}

template<typename T>
bool ImageGridComponent<T>::ProcessInput(const InputCompactEvent& event)
{
	if (event.AnythingPressed())
	{
		Vector2i dir = Vector2i::Zero();
		if (event.AnyUpPressed()) dir.y() = -1;
		else if (event.AnyDownPressed()) dir.y() = 1;
		else if (event.AnyLeftPressed()) dir.x() = -1;
		else if (event.AnyRightPressed()) dir.x() = 1;

		if(dir != Vector2i::Zero())
		{
			listInput(dir.x() + dir.y() * getGridSize().x());
			return true;
		}
	}
	else if (event.AnyUpReleased() || event.AnyDownReleased() || event.AnyLeftReleased() || event.AnyRightReleased())
  {
    stopScrolling();
  }

	return Component::ProcessInput(event);
}

template<typename T>
void ImageGridComponent<T>::Update(int deltaTime)
{
	listUpdate(deltaTime);
}

template<typename T>
void ImageGridComponent<T>::Render(const Transform4x4f& parentTrans)
{
  Transform4x4f trans = getTransform() * parentTrans;

	if(mEntriesDirty)
	{
		buildImages();
		updateImages();
		mEntriesDirty = false;
	}

	for (auto& image : mImages)
	{
    image.Render(trans);
	}

	Component::renderChildren(trans);
}

template<typename T>
void ImageGridComponent<T>::onCursorChanged(const CursorState& state)
{
	(void)state;
	updateImages();
}

template<typename T>
void ImageGridComponent<T>::onSizeChanged()
{
	buildImages();
	updateImages();
}

// create and position imagecomponents (mImages)
template<typename T>
void ImageGridComponent<T>::buildImages()
{
	mImages.clear();

	Vector2i gridSize = getGridSize();
	Vector2f squareSize = getMaxSquareSize();
	Vector2f padding = getPadding();

	// attempt to center within our size
	Vector2f totalSize((float)gridSize.x() * (squareSize.x() + padding.x()), (float)gridSize.y() * (squareSize.y() + padding.y()));
	Vector2f offset(mSize.x() - totalSize.x(), mSize.y() - totalSize.y());
	offset /= 2;

	for (int y = 0; y < gridSize.y(); y++)
	{
		for (int x = 0; x < gridSize.x(); x++)
		{
			mImages.push_back(ImageComponent(mWindow));
			ImageComponent& image = mImages[y * gridSize.x() + x];

			image.setPosition((squareSize.x() + padding.x()) * ((float)x + 0.5f) + offset.x(), (squareSize.y() + padding.y()) * ((float)y + 0.5f) + offset.y());
			image.setOrigin(0.5f, 0.5f);
			image.setResize(squareSize.x(), squareSize.y());
			image.setImage(Path());
		}
	}
}

template<typename T>
void ImageGridComponent<T>::updateImages()
{
	if(mImages.empty())
		buildImages();

	Vector2i gridSize = getGridSize();

	int cursorRow = mCursor / gridSize.x();
	//int cursorCol = mCursor % gridSize.x();

	int start = (cursorRow - (gridSize.y() / 2)) * gridSize.x();

	//if we're at the end put the row as close as we can and no higher
	if(start + (gridSize.x() * gridSize.y()) >= (int)mEntries.size())
		start = gridSize.x() * ((int)mEntries.size()/gridSize.x() - gridSize.y() + 1);

	if(start < 0)
		start = 0;

	unsigned int i = (unsigned int)start;
	for (unsigned int img = 0; img < mImages.size(); img++)
	{
		ImageComponent& image = mImages[img];
		if(i >= (unsigned int)size())
		{
			image.setImage(Path());
			continue;
		}

		Vector2f squareSize = getSquareSize(mEntries[i].data.texture);
		if((int)i == mCursor)
		{
			image.setColorShift(0xFFFFFFFF);
			image.setResize(squareSize.x() + getPadding().x() * 0.95f, squareSize.y() + getPadding().y() * 0.95f);
		}else{
			image.setColorShift(0xAAAAAABB);
			image.setResize(squareSize.x(), squareSize.y());
		}

		image.setImage(mEntries[i].data.texture);
		i++;
	}
}
