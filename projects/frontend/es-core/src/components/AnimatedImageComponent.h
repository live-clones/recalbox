#include "components/base/Component.h"
#include "ImageComponent.h"

struct AnimationFrame
{
	const char* path;
	int time;
};

struct AnimationDef
{
	AnimationFrame* frames;
	size_t frameCount;
	bool loop;
};

class AnimatedImageComponent : public Component
{
public:
	explicit AnimatedImageComponent(WindowManager&window);
	
	void load(const AnimationDef* def); // no reference to def is kept after loading is complete

	void reset(); // set to frame 0

	void Update(int deltaTime) override;
	void Render(const Transform4x4f& trans) override;

	void onSizeChanged() override;

private:
	typedef std::pair<std::unique_ptr<ImageComponent>, int> ImageFrame;

	std::vector<ImageFrame> mFrames;

	bool mLoop;
	bool mEnabled;
	int mFrameAccumulator;
	int mCurrentFrame;
};
