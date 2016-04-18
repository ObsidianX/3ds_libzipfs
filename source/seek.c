#include <unistd.h>
#include <errno.h>

#include "zipfs_internal.h"

off_t zipfs_seek(struct _reent *r, int fd, off_t pos, int dir) {
    zipfs_file_t *file = (zipfs_file_t *) fd;

    // only allow seeking within the file's data
    off_t actual_pos = pos;
    switch (dir) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            actual_pos += file->position;
            break;
        case SEEK_END:
            actual_pos += file->filesize;
            break;
        default:
            errno = EINVAL;
            return -1;
    }

    if (actual_pos < 0 || actual_pos >= file->filesize) {
        errno = EINVAL;
        return -1;
    }

    off_t new_pos = lseek(file->fd, pos, dir) - file->zip_offset;
    file->position = (uint32_t) new_pos;
    return new_pos;
}