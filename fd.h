namespace InstantSend {

class Fd {
	public:
		explicit Fd(int fd);
		~Fd();
		operator int();
		int release();
	private:
		int operator=(const Fd&);
		int mFd;
};

inline Fd::Fd(int fd) : mFd(fd) {}
inline Fd::~Fd() { if(mFd >= 0) close(mFd); }
inline Fd::operator int() { return mFd; }
inline int Fd::release() { int tmp = mFd; mFd = -1; return tmp; }

}
