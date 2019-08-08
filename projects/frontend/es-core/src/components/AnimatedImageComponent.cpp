#include "components/AnimatedImageComponent.h"
#include "Log.h"
#include "MenuThemeData.h"

AnimatedImageComponent::AnimatedImageComponent(Window* window)
  : GuiComponent(window), mLoop(false), mEnabled(false), mFrameAccumulator(0), mCurrentFrame(0)
{
}

void AnimatedImageComponent::load(const AnimationDef* def)
{
	mFrames.clear();
	auto menuTheme = MenuThemeData::getInstance()->getCurrentTheme();
	assert(def->frameCount >= 1);

	for (size_t i = 0; i < def->frameCount; i++)
	{
		if(def->frames[i].path != nullptr && !ResourceManager::getInstance()->fileExists(def->frames[i].path))
		{
			LOG(LogError) << "Missing animation frame " << i << " (\"" << def->frames[i].path << "\")";
			continue;
		}

		auto img = std::unique_ptr<ImageComponent>(new ImageComponent(mWindow));
		img->setResize(mSize.x(), mSize.y());
		img->setImage(std::string(def->frames[i].path), false);
		img->setColorShift(menuTheme->menuText.color);
		mFrames.push_back(ImageFrame(std::move(img), def->frames[i].time));
	}

	mLoop = def->loop;

	mCurrentFrame = 0;
	mFrameAccumulator = 0;
	mEnabled = true;
}

void AnimatedImageComponent::reset()
{
	mCurrentFrame = 0;
	mFrameAccumulator = 0;
}

void AnimatedImageComponent::onSizeChanged()
{
	for (auto& mFrame : mFrames)
	{
		mFrame.first->setResize(mSize.x(), mSize.y());
	}
}

void AnimatedImageComponent::update(int deltaTime)
{
	if(!mEnabled || mFrames.empty())
		return;

	mFrameAccumulator += deltaTime;

	while(mFrames.at(mCurrentFrame).second <= mFrameAccumulator)
	{
		mCurrentFrame++;

		if(mCurrentFrame == (int)mFrames.size())
		{
			if(mLoop)
			{
				// restart
				mCurrentFrame = 0;
			}else{
				// done, stop at last frame
				mCurrentFrame--;
				mEnabled = false;
				break;
			}
		}

		mFrameAccumulator -= mFrames.at(mCurrentFrame).second;
	}
}

void AnimatedImageComponent::render(const Transform4x4f& trans)
{
	if (!mFrames.empty())
		mFrames.at(mCurrentFrame).first->render(getTransform() * trans);
}
