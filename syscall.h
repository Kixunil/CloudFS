#include <stdexcept>
#include <errno.h>
#include <string>
#include <cstring>

#define SC_CHK(SC, ...) syscall_check(SC(__VA_ARGS__), #SC ": ")

inline int64_t syscall_check(int64_t ret, const std::string &errmsg) { if(ret < 0) throw std::runtime_error(errmsg + strerror(errno)); return ret; }
