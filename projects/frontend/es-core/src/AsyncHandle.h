#pragma once

enum class AsyncStatus
{
	InProgress,
	Error,
	Done,
};

// Handle for some asynchronous operation.
class AsyncHandle
{
public:
	AsyncHandle()
	  : mStatus(AsyncStatus::InProgress)
	{
	}
	virtual ~AsyncHandle() = default;

	virtual void update() = 0;

	// Update and return the latest status.
	inline AsyncStatus status() { update(); return mStatus; }

	// User-friendly string of our current status.  Will return error message if status() == SEARCH_ERROR.
	inline std::string getStatusString()
	{
		switch(mStatus)
		{
		case AsyncStatus::InProgress:
			return "in progress";
		case AsyncStatus::Error:
			return mError;
		case AsyncStatus::Done:
			return "done";
		default:
			return "something impossible has occured; row, row, fight the power";
		}
	}

protected:
	inline void setStatus(AsyncStatus status) { mStatus = status; }
	inline void setError(const std::string& error) { setStatus(AsyncStatus::Error); mError = error; }

	std::string mError;
  AsyncStatus mStatus;
};
