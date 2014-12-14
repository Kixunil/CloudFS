#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "network.h"

#include "syscall.h"
#include "fd.h"

using namespace std;

namespace InstantSend {

class PosixConnection : public Connection::Data {
	public:
		PosixConnection(int fd, const string &addr, unsigned short port);
		size_t read(void *data, size_t size);
		void write(const void *data, size_t size);
		void shutdown(Connection::SDType type);
	private:
		Fd mFd;
};

class PosixListener : public Listener::Data {
	public:
		PosixListener(int fd);
		Connection accept();
	private:
		Fd mFd;
};

inline PosixConnection::PosixConnection(int fd, const string &addr, unsigned short port) : Connection::Data(addr, port), mFd(fd) {}

Connection::Connection(const string &host, unsigned short port, long timeout_ms) {
	struct addrinfo *ainf = NULL, *p = NULL, hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(host.c_str(), NULL, &hints, &ainf);
	if(ret) {
		throw runtime_error(string("getaddrinfo: ") + gai_strerror(ret));
	}
	
	p = ainf;
	try {
		while(p) {
			switch(p->ai_family) {
				case AF_INET:
					((struct sockaddr_in *)p->ai_addr)->sin_port = htons(port);
					break;
				case AF_INET6:
					((struct sockaddr_in6 *)p->ai_addr)->sin6_port = htons(port);
					break;
				default:
					continue;
			}
			Fd fd(SC_CHK(::socket, p->ai_family, p->ai_socktype, p->ai_protocol));
			try {
				SC_CHK(::connect, fd, p->ai_addr, p->ai_addrlen);
				mData = new PosixConnection(fd, host, port);
				fd.release();
				mData->incRC();
				freeaddrinfo(ainf);
				return;
			} catch(...) {
				p = p->ai_next;
			}
		}
		if(!p) throw runtime_error("Can't connect");
	} catch(...) {
		freeaddrinfo(ainf);
		throw;
	}
}

size_t PosixConnection::read(void *data, size_t size) {
	size_t len = SC_CHK(::read, mFd, data, size);
	if(!len) throw Connection::End();
	return len;
}

void PosixConnection::write(const void *data, size_t size) {
	const char *p = (const char *)data;

	while(size) {
		ssize_t len = ::write(mFd, p, size);
		if(len < 0 && errno == EINTR) { errno = 0; continue; }
		syscall_check(len, "write: ");
		size -= len;
		p += len;
	}
}

void PosixConnection::shutdown(Connection::SDType type) {
	int how;
	switch(type) {
		case Connection::Read:
			how = SHUT_RD;
			break;
		case Connection::Write:
			how = SHUT_WR;
			break;
		case Connection::Both:
			how = SHUT_RDWR;
			break;

	}

	SC_CHK(::shutdown, mFd, how);
}

inline PosixListener::PosixListener(int fd) : mFd(fd) {}

Connection PosixListener::accept() {
	struct sockaddr_in saddr;
	socklen_t addrlen(sizeof(saddr));
	Fd fd(SC_CHK(::accept, mFd, (struct sockaddr *)&saddr, &addrlen));

	Connection conn(new PosixConnection(fd, inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port)));
	fd.release();

	return conn;
}

int bind_ipv4(unsigned short port) {
	Fd fd(SC_CHK(socket, AF_INET, SOCK_STREAM, 0));

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	SC_CHK(bind, fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
	return fd.release();
}

int bind_ipv6(unsigned short port) {
	throw runtime_error("IPv6 not implemented");
}

Listener::Listener(unsigned short port, Type type) {
	Fd fd(type == IPv4?bind_ipv4(port):bind_ipv6(port));
	SC_CHK(::listen, fd, 42);
	mData = new PosixListener(fd);
	fd.release();
	mData->incRC();
}

}
