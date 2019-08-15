#pragma once

#include <curl/curl.h>
#include <sstream>
#include <map>

/* Usage:
 * HttpReq myRequest("www.google.com", "/index.html");
 * //for blocking behavior: while(myRequest.status() == HttpReq::REQ_IN_PROGRESS);
 * //for non-blocking behavior: check if(myRequest.status() != HttpReq::REQ_IN_PROGRESS) in some sort of update method
 * 
 * //once one of those completes, the request is ready
 * if(myRequest.status() != REQ_SUCCESS)
 * {
 *    //an error occured
 *    LOG(LogError) << "HTTP request error - " << myRequest.getErrorMessage();
 *    return;
 * }
 *
 * std::string content = myRequest.getContent();
 * //process contents...
*/

class HttpReq
{
public:
	explicit HttpReq(const std::string& url);

	~HttpReq();

	enum class Status
	{
		InProgress,      //request is in progress
		Success,         //request completed successfully, get it with getContent()

		IOError,         //some boost::asio error happened, get it with getErrorMsg()
		BadStatus,       //some invalid HTTP response status code happened (non-200)
		InvalidResponse, //the HTTP response was invalid
	};

	Status status(); //process any received data and return the status afterwards

  std::string getErrorMsg() { return mErrorMsg; }

	std::string getContent() const; // mStatus must be REQ_SUCCESS

	static std::string urlEncode(const std::string &s);
	static bool isUrl(const std::string& s);

private:
	static size_t write_content(void* buff, size_t size, size_t nmemb, void* req_ptr);
	//static int update_progress(void* req_ptr, double dlTotal, double dlNow, double ulTotal, double ulNow);

	//god dammit libcurl why can't you have some way to check the status of an individual handle
	//why do I have to handle ALL messages at once
	static std::map<CURL*, HttpReq*> s_requests;

	static CURLM* s_multi_handle;

  void onError(const char* msg) { mErrorMsg = msg; }

	CURL* mHandle;

	Status mStatus;

	std::stringstream mContent;
	std::string mErrorMsg;
};
