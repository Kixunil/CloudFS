#include "rc.h"

using namespace InstantSend;

RefCnt::~RefCnt() {}

void RefCnt::incRC() {
	__sync_fetch_and_add(&mRC, 1);
}

bool RefCnt::decRC() {
	return __sync_fetch_and_sub(&mRC, 1) == 1;
}
