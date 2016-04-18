#include <errno.h>
#include <unistd.h>

#include "zipfs_internal.h"

ssize_t zipfs_read(struct _reent *r, int fd, char *ptr, size_t len) {
    zipfs_file_t *file = (zipfs_file_t *) fd;

    off_t orig_pos = lseek(file->fd, 0, SEEK_CUR);

    lseek(file->fd, file->zip_offset + file->position, SEEK_SET);

    zip_header_signature_t signature;
    if (zip_read_signature(file->fd, &signature) && signature.type == HEADER_LOCAL_FILE) {
        zip_local_file_header_t local_file;
        if (zip_read_local_file(file->fd, &local_file)) {
            // skip filename and extras
            lseek(file->fd, local_file.filename_len + local_file.extra_len, SEEK_CUR);

            if (len + file->position > file->filesize) {
                len = file->filesize - file->position;
            }
            ssize_t bytes = read(file->fd, ptr, len);
            file->position += bytes;

            return bytes;
        }
    }

    lseek(file->fd, orig_pos, SEEK_SET);
    errno = ENOENT;
    return -1;
}