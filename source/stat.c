#include <fcntl.h>

#include "zipfs_internal.h"

int zipfs_stat(struct _reent *r, const char *path, struct stat *st) {
    zipfs_file_t file;
    zipfs_open(r, &file, path, O_RDONLY, O_RDONLY);

    zipfs_fstat(r, (int) &file, st);

    return 0;
}