#include <stdint.h>

#include <string>

#include "http.h"

class CloudFile {
	public:
		CloudFile(const URL &url);
		size_t read(void *data, size_t size);
		void seek(uint64_t where);
		uint64_t size();
		uint64_t offset();

		bool seekable() const;
	private:
		URL mUrl;
		uint64_t mSize, mOffset;
		bool mSeekable;
		InstantSend::Connection mConn;

		HTTPResponse sendReq(HTTPRequest &request);
};

inline void CloudFile::seek(uint64_t where) {
	mOffset = where;
}

inline uint64_t CloudFile::size() {
	return mSize;
}

inline uint64_t CloudFile::offset() {
	return mOffset;
}

inline bool CloudFile::seekable() const {
	return mSeekable;
}
