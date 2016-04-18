#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "zip.h"

bool zip_find_path(int fd, zip_end_central_dir_header_t *central_dir, const char *path, zip_central_dir_header_t *out) {
    // save position
    off_t orig_pos = lseek(fd, 0, SEEK_CUR);

    lseek(fd, central_dir->offset, SEEK_SET);

    bool res = false;
    zip_central_dir_header_t cur;
    zip_header_signature_t header;
    int bytes;
    char cur_name[FILENAME_MAX];
    off_t seek_by;

    while (true) {
        if (!zip_read_signature(fd, &header) || header.type != HEADER_CENTRAL_DIR) {
            break;
        }

        if (!zip_read_central_dir(fd, &cur)) {
            break;
        }

        bytes = read(fd, cur_name, cur.filename_len);
        if (bytes != cur.filename_len) {
            break;
        }

        if (!strncmp(cur_name, path, cur.filename_len)) {
            *out = cur;
            res = true;
            break;
        }

        // skip extra data and comment
        seek_by = cur.extra_len + cur.comment_len;

        lseek(fd, seek_by, SEEK_CUR);
    }

    // restore position
    lseek(fd, orig_pos, SEEK_SET);

    return res;
}