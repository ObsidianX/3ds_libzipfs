#include <errno.h>
#include <unistd.h>

#include "zipfs_internal.h"

#define ENOTFILE    __ELASTERROR + 1
#define EINVALENT   __ELASTERROR + 2

ssize_t zipfs_read(struct _reent *r, int fd, char *ptr, size_t len) {
    zipfs_file_t *file = (zipfs_file_t *) fd;

    off_t orig_pos = lseek(file->fd, 0, SEEK_CUR);

    lseek(file->fd, file->zip_offset, SEEK_SET);

    zip_header_signature_t signature;
    if (zip_read_signature(file->fd, &signature) && signature.type == HEADER_LOCAL_FILE) {
        zip_local_file_header_t local_file;
        if (zip_read_local_file(file->fd, &local_file)) {
            // skip filename and extras
            lseek(file->fd, local_file.filename_len + local_file.extra_len + file->position, SEEK_CUR);

            if (len + file->position > file->filesize) {
                len = file->filesize - file->position;
            }
            ssize_t bytes = read(file->fd, ptr, len);
            file->position += bytes;

            return bytes;
        } else {
            r->_errno = EINVALENT;
        }
    } else {
        r->_errno = ENOTFILE;
    }

    lseek(file->fd, orig_pos, SEEK_SET);
    return -1;
}