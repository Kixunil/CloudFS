#ifndef _INSTANTSEND_RC_INCLUDED
#define _INSTANTSEND_RC_INCLUDED

#define INSTANTSEND_AUTORC_ARG(TYPE, VISIBILITY) \
	public: \
		inline TYPE(const TYPE &other) : mData(other.mData) { mData->incRC(); } \
		TYPE &operator=(const TYPE &other); \
		inline ~TYPE() { if(mData->decRC()) delete mData; } \
	VISIBILITY: \
		Data *mData;


#define INSTANTSEND_AUTORC(TYPE) INSTANTSEND_AUTORC_ARG(TYPE, private)

#define INSTANTSEND_AUTORC_DPROT(TYPE) INSTANTSEND_AUTORC_ARG(TYPE, protected)

#define INSTANTSEND_AUTORC_IMPLEMENT(TYPE) \
	TYPE &TYPE::operator=(const TYPE &other) { \
		other.mData->incRC(); \
		mData->decRC(); \
		mData = other.mData; \
		return *this; \
	} \

namespace InstantSend {

/// Base class used for reference counting.
class RefCnt {
	public:
		/// Constructor.
		/// Initializes reference count to zero
		RefCnt();

		//Destructor
		virtual ~RefCnt();

		/// Atomicaly increases reference count.
		void incRC();

		/// Atomicaly decreases reference count.
		/// \return True if count reached zero
		bool decRC();
	private:
		volatile unsigned int mRC;
};


inline RefCnt::RefCnt() : mRC(0) {}

}

#endif
