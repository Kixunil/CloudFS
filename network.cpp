#include "network.h"

namespace InstantSend {

const char *Connection::End::what() const throw() {
	return "Connection closed";
}

Connection::End::~End() throw() {}

INSTANTSEND_AUTORC_IMPLEMENT(Connection);
INSTANTSEND_AUTORC_IMPLEMENT(Listener);

}
