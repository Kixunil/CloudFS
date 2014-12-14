#include "cloudfile.h"

#include <cstdio>
#include <stdexcept>

using namespace std;
using namespace InstantSend;

CloudFile::CloudFile(const URL &url) : mUrl(url), mOffset(0) {
	Connection conn(url.host(), url.port());
	HTTPRequest request(HTTPRequest::HEAD, url);
	request["Range"] = "bytes=0-";
	request.send(conn);

	HTTPResponse response(conn);
	if(response.code() != 200 && response.code() != 206)
		throw runtime_error("HTTP error: " + response.respStr());

	sscanf(response["content-length"].c_str(), "%lu", &mSize);

	try {
		mSeekable = response["accept-ranges"] == "bytes";
	} catch(...) {
		mSeekable = false;
	}
}

size_t CloudFile::read(void *data, size_t size) {
	Connection conn(mUrl.host(), mUrl.port());
	HTTPRequest request(HTTPRequest::GET, mUrl);
	request["Range"] = "bytes=";
	char buf[30];
	sprintf(buf, "%lu", mOffset);
	request["Range"] += buf;
	request["Range"] += "-";
	sprintf(buf, "%lu", mOffset + size);
	request["Range"] += buf;
	request.send(conn);
	HTTPResponse response(conn);
	
	unsigned long len;
	sscanf(response["content-length"].c_str(), "%lu", &len);

	if(len < size) size = len;

	fprintf(stderr, "Reading %zu bytes\n", size);

	size_t r, total = 0;
	while(size) {
		r = conn.read(data, size);
		total += r;
		size -= r;
		*(char **)&data += r;
	}

	mOffset += total;
	return total;
}
