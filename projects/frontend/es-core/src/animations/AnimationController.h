#pragma once

#include <functional>
#include "animations/Animation.h"

class AnimationController
{
public:
	// Takes ownership of anim (will delete in destructor).
	explicit AnimationController(Animation* anim, int delay, const std::function<void()>& finishedCallback, bool reverse);
	virtual ~AnimationController();

	// Returns true if the animation is complete.
	bool update(int deltaTime);

	inline bool isReversed() const { return mReverse; }
	inline int getTime() const { return mTime; }
	inline Animation* getAnimation() const { return mAnimation; }

	inline void removeFinishedCallback() { mFinishedCallback = nullptr; }

private:
	Animation* mAnimation;
	std::function<void()> mFinishedCallback;
	bool mReverse;
	int mTime;
	int mDelay;
};
