#include "http.h"

#include <cstdio>
#include <algorithm>
#include<stdexcept>

using namespace std;
using namespace InstantSend;

URL::URL(const std::string &url) : mUrl(url) {
	parse();
}

URL::URL(const char *url) : mUrl(url) {
	parse();
}

void URL::parse() {
	size_t offset = 0;
	if(mUrl.substr(0, 8) == "https://") {
		mSecure = true;
		offset = 8;
	} else {
		mSecure = false;
		if(mUrl.substr(0, 7) == "http://") {
			 offset = 7;
		}
	}

	size_t slash = mUrl.find('/', offset);
	if(slash == string::npos) {
		mPath = '/';
		slash = mUrl.size();
	} else {
		mPath = mUrl.substr(slash);
	}

	size_t colon = mUrl.find(':', offset);
	
	if(colon > slash) {
		mHost = mUrl.substr(offset, slash - offset);
		mPort = 80;
	} else {
		mHost = mUrl.substr(offset, colon - offset);
		sscanf(mUrl.substr(colon + 1, slash - colon - 1).c_str(), "%hu", &mPort);
	}
}

const std::string HTTPRequest::methodStr[] = {
	"HEAD", "GET", "POST", "PUT",
	"DELETE", "OPTIONS", "TRACE"
};

HTTPRequest::HTTPRequest(Method method, const URL &url) :
	mFirst(HTTPRequest::methodStr[method] + " " + url.path() + " HTTP/1.1\r\n") {
	mFields["Host"] = url.host();
}

void HTTPRequest::send(Connection conn) {
	string out(mFirst);
	map<std::string, std::string>::iterator i(mFields.begin());
	for(; i != mFields.end(); ++i) {
		out += i->first + ": " + i->second + "\r\n";
	}
	out += "\r\n";

	conn.write(out.c_str(), out.size());
}

std::string HTTPResponse::readLine(Connection conn) {
	std::string line;
	char c = 0;
	while(c != '\n') {
		conn.read(&c, 1);
		line += c;
	}
	return line;
}

HTTPResponse::HTTPResponse(Connection conn) {
	std::string line(readLine(conn));
	// HTTP/1.1 200 OK
	mVer = line.substr(5, 3);
	mRS = line.substr(13);
	sscanf(line.substr(9, 3).c_str(), "%d", &mCode);
	while((line = readLine(conn)).size() > 2) {
		size_t colon = line.find(':');
		std::transform(line.begin(), line.begin() + colon, line.begin(), ::tolower);
		mFields[line.substr(0, colon)] = line.substr(colon + 2);
	}
}

const std::string &HTTPResponse::operator [](const std::string &key) const {
	map<std::string, std::string>::const_iterator f(mFields.find(key));
	if(f == mFields.end()) throw std::runtime_error("Field not found");
	return f->second;
}
