#ifndef _INSTANTSEND_NETWORK_INCLUDED
#define _INSTANTSEND_NETWORK_INCLUDED

#include <string>

#include "rc.h"

namespace InstantSend {

/// Platform independent network connection.
class Connection {
	public:
		/// Exception thrown if connection is closed.
		class End : public std::exception {
			public:
				const char *what() const throw();
				~End() throw();
		};

		/// Shutdown type.
		typedef enum { Read, Write, Both } SDType;

		/// Implementation of the connection.
		class Data : public RefCnt {
			public:
				Data(const std::string &addr, unsigned short port);
				virtual size_t read(void *data, size_t size) = 0;
				virtual void write(const void *data, size_t size) = 0;
				virtual void shutdown(SDType type) = 0;

				const std::string &peerAddr() const;
				unsigned short peerPort() const;


				using RefCnt::incRC;
				using RefCnt::decRC;
			private:
				std::string mAddr;
				unsigned short mPort;
		};

		/// Constructor.
		/// \param host Remote machine to connect to
		/// \param port Remote port
		/// \param timeout_ms Limits time to give up connecting. -1 means system default
		Connection(const std::string &host, unsigned short port, long timeout_ms = -1);

		/// Constructor.
		/// \param data Connection implementation
		Connection(Data *data);

		/// Receives data from peer.
		/// \param data Destination buffer in which data should be put
		/// \param size Maximum number of bytes to be read
		size_t read(void *data, size_t size);

		/// Sends data to peer.
		/// \param data Data to be sent
		/// \param size Data size
		void write(const void *data, size_t size);

		/// Informs peer that connection will be closed.
		/// \param type Specifies which operations will not be performed after call of this function
		void shutdown(SDType type);

		/// Peer address getter.
		/// \return remote address
		const std::string &peerAddr() const;

		/// Port getter.
		/// \return remote port
		unsigned short peerPort() const;

		INSTANTSEND_AUTORC(Connection);
};

/// Holds bound listening socket.
class Listener {
	public:
		/// Defines connection types.
		/// IPv6 not supported yet.
		typedef enum { IPv4, IPv6 } Type;

		/// Interface for real implementation
		class Data : public RefCnt {
			public:
				/// Waits for new client.
				/// \return Opened connection with client
				virtual Connection accept() = 0;
		};

		/// Constructor.
		/// Opens given port.
		Listener(unsigned short port, Type type = IPv4);
		//Listener(const std::string &addr, unsigned short port);

		/// Waits for new client.
		/// \return Opened connection with client
		Connection accept();

		INSTANTSEND_AUTORC(Listener);
};

inline Connection::Connection(Data *data) :mData(data) {
	mData->incRC();
}

inline size_t Connection::read(void *data, size_t size) {
	return mData->read(data, size);
}

inline void Connection::write(const void *data, size_t size) {
	mData->write(data, size);
}

inline void Connection::shutdown(SDType type) {
	mData->shutdown(type);
}

inline Connection Listener::accept() {
	return mData->accept();
}

inline Connection::Data::Data(const std::string &addr, unsigned short port) : mAddr(addr), mPort(port) {}

inline const std::string &Connection::peerAddr() const {
	return mData->peerAddr();
}

inline unsigned short Connection::peerPort() const {
	return mData->peerPort();
}

inline const std::string &Connection::Data::peerAddr() const {
	return mAddr;
}

inline unsigned short Connection::Data::peerPort() const {
	return mPort;
}

}

#endif
