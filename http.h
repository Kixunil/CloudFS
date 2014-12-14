#include <stdint.h>

#include <string>
#include <map>

#include "network.h"

class URL {
	public:
		URL(const std::string &url);
		URL(const char *url);
		const std::string &url() const;
		const std::string &host() const;
		uint16_t port() const;
		const std::string &path() const;
		bool secure() const;
	private:
		void parse();
		std::string mUrl;
		std::string mHost;
		uint16_t mPort;
		std::string mPath;
		bool mSecure;
};

class HTTPRequest {
	public:
		typedef enum {
			HEAD	= 0,
			GET	= 1,
			POST	= 2,
			PUT	= 3,
			DELETE	= 4,
			OPTIONS	= 5,
			TRACE	= 6
			// , CONNECT	= 7 ?
		} Method;

		static const std::string methodStr[];

		HTTPRequest(Method method, const URL &url);
		std::string &operator [](const std::string &key);
		void send(InstantSend::Connection conn);
	private:
		std::string mFirst;
		std::map<std::string, std::string> mFields;
};

class HTTPResponse {
	public:
		HTTPResponse(InstantSend::Connection conn);
		const std::string &version() const;
		int code() const;
		const std::string &respStr();
		const std::string &operator [](const std::string &key) const;
	private:
		static std::string readLine(InstantSend::Connection conn);
		std::string mVer, mRS;
		int mCode;
		std::map<std::string, std::string> mFields;
};

inline const std::string &URL::url() const {
	return mUrl;
}

inline const std::string &URL::host() const {
	return mHost;
}

inline uint16_t URL::port() const {
	return mPort;
}

inline const std::string &URL::path() const {
	return mPath;
}

inline bool URL::secure() const {
	return mSecure;
}

inline std::string &HTTPRequest::operator [](const std::string &key) {
	return mFields[key];
}

inline const std::string &HTTPResponse::version() const {
	return mVer;
}

inline int HTTPResponse::code() const {
	return mCode;
}

inline const std::string &HTTPResponse::respStr() {
	return mRS;
}
