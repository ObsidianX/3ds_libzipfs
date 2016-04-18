#include <string.h>

#include "zipfs_internal.h"

int zipfs_fstat(struct _reent *r, int fd, struct stat *st) {
    zipfs_file_t *file = (zipfs_file_t *) fd;

    bzero(st, sizeof(struct stat));
    st->st_size = file->filesize;
    st->st_mode = file->is_dir ? _IFDIR : _IFREG;

    return 0;
}