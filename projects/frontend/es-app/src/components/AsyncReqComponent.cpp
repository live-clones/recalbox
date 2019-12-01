#include <Locale.h>
#include "components/AsyncReqComponent.h"
#include "Renderer.h"

AsyncReqComponent::AsyncReqComponent(Window& window, std::shared_ptr<HttpReq> req, std::function<void(std::shared_ptr<HttpReq>)> onSuccess, std::function<void()> onCancel)
	: Gui(window),
    mSuccessFunc(std::move(onSuccess)),
    mCancelFunc(std::move(onCancel)),
    mTime(0),
    mRequest(std::move(req))
{
	
}

bool AsyncReqComponent::ProcessInput(const InputCompactEvent& event)
{
	if (event.APressed())
	{
		if(mCancelFunc)
			mCancelFunc();

		Close();
	}

	return true;
}

void AsyncReqComponent::Update(int deltaTime)
{
	if(mRequest->status() != HttpReq::Status::InProgress)
	{
		mSuccessFunc(mRequest);
		Close();
		return;
	}

	mTime += deltaTime;
}

void AsyncReqComponent::Render(const Transform4x4f& parentTrans)
{
	(void)parentTrans;

	Transform4x4f trans = Transform4x4f::Identity();
	trans = trans.translate(Vector3f(Renderer::getDisplayWidthAsFloat() / 2.0f, Renderer::getDisplayHeightAsFloat() / 2.0f, 0));
	Renderer::setMatrix(trans);

	Vector3f point(cos((float)mTime * 0.01f) * 12, sin((float)mTime * 0.01f) * 12, 0);
	Renderer::drawRect((int)point.x(), (int)point.y(), 8, 8, 0x0000FFFF);
}

bool AsyncReqComponent::getHelpPrompts(Help& help)
{
	help.Set(HelpType::A, _("CANCEL"));
	return true;
}
