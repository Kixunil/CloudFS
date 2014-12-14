#define _FILE_OFFSET_BITS 64
#include <errno.h>

#include <cstdio>
#include <map>

#define FUSE_USE_VERSION 26
#include <fuse.h>

#include "cloudfile.h"

using namespace std;

//FILE * log;

map<string, string> files;

inline const CloudFile &ffiToCf(struct fuse_file_info *fi) {
	return *(CloudFile *)fi->fh;
}

inline void CfToFfi(const CloudFile &cf, struct fuse_file_info *fi) {
	fi->fh = (uint64_t) new CloudFile(cf);
}

int CloudFS_getattr(const char *path, struct stat *st) {
	//fprintf(log, "getattr(%s)\n", path);
	if(string(path) == "/") {
		st->st_mode = S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH;
		st->st_nlink = 2;
		st->st_uid = 0;
		st->st_gid = 0;
		st->st_size = 0;
		st->st_blksize = 0;
		st->st_blocks = 0;
		st->st_atime = 0;
		st->st_mtime = 0;
		st->st_ctime = 0;
		return 0;
	}

	map<string, string>::iterator f(files.find(path));
	if(f == files.end()) return -ENOENT;
	try {
		CloudFile file(f->second);
		
		st->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;
		st->st_nlink = 1;
		st->st_uid = 0;
		st->st_gid = 0;
		st->st_size = file.size();
		st->st_blksize = 4096;
		st->st_blocks = st->st_size / 512 + (st->st_size % 512 > 0);
		st->st_atime = 0;
		st->st_mtime = 0;
		st->st_ctime = 0;
		return 0;
	} catch(...) { 
		return -ENOENT;
	}
}

int CloudFS_open(const char *path, struct fuse_file_info *fi) {
	try {
		map<string, string>::iterator f(files.find(path));
		if(f == files.end()) return -ENOENT;
		CloudFile file(f->second);
		//fi->nonseekable = ! f.seekable();
		CfToFfi(file, fi);
		return 0;
	} catch(...) {
		return -ENOENT;
	}
}

int CloudFS_release(const char *, struct fuse_file_info *fi) {
	delete &ffiToCf(fi);
	return 0;
}

int CloudFS_read(const char *, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	CloudFile cf(ffiToCf(fi));
	cf.seek(offset);
	return cf.read(buf, size);
}

int CloudFS_write(const char *, const char *, size_t, off_t, struct fuse_file_info *) {
	return -EBADF;
}

int CloudFS_symlink(const char *dst, const char *path) {
	files[path] = dst;
	return 0;
}

int CloudFS_getdir(const char *, fuse_dirh_t dh, fuse_dirfil_t filler) {
	map<string, string>::iterator it(files.begin());
	for(; it != files.end(); ++it) {
		if(filler(dh, it->first.c_str() + 1, S_IFREG, 0)) {
			return -ENOMEM;
		}
	}
	return 0;
}

struct fuse_operations CloudFS_oper;

int main(int argc, char **argv) {
	//log = fopen("cloudfs.log", "w");

	CloudFS_oper.getattr = CloudFS_getattr;
	CloudFS_oper.open = CloudFS_open;
	CloudFS_oper.release = CloudFS_release;
	CloudFS_oper.read = CloudFS_read;
	CloudFS_oper.write = CloudFS_write;
	CloudFS_oper.getdir = CloudFS_getdir;
	CloudFS_oper.symlink = CloudFS_symlink;

	fuse_main(argc, argv, &CloudFS_oper, NULL);

	//fclose(log);

	return 0;
}
