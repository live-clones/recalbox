#pragma once

#include "components/base/Component.h"

class ScrollableContainer : public Component
{
public:
	explicit ScrollableContainer(WindowManager&window);

	Vector2f getScrollPos() const { return mScrollPos; }
	void setScrollPos(const Vector2f& pos) { mScrollPos = pos; }

  void setAutoScroll(bool autoScroll);
	void reset();

	void Update(int deltaTime) override;
	void Render(const Transform4x4f& parentTrans) override;
	int mAutoScrollDelay; // ms to wait before starting to autoscroll
	int mAutoScrollSpeed; // ms to wait before scrolling down by mScrollDir
	int mAutoScrollResetAccumulator;
	int mAutoScrollAccumulator;


private:
	Vector2f getContentSize();

	Vector2f mScrollPos;
	Vector2f mScrollDir;
	bool mAtEnd;

};
