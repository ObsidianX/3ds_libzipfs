#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "zip.h"

#define EODC_COMMENT_MAX 0xFFFF

bool zip_read_signature(int fd, zip_header_signature_t *out) {
    errno = 0;
    int bytes = read(fd, &out->raw, sizeof(zip_header_signature_t));
    if (errno != 0) {
        return false;
    }
    if (bytes != sizeof(zip_header_signature_t)) {
        errno = EOF;
        return false;
    }

    return true;
}

bool zip_read_local_file(int fd, zip_local_file_header_t *out) {
    errno = 0;
    int bytes = read(fd, out, sizeof(zip_local_file_header_t));
    if (errno != 0) {
        return false;
    }
    if (bytes != sizeof(zip_local_file_header_t)) {
        errno = EOF;
        return false;
    }

    return true;
}

bool zip_read_central_dir(int fd, zip_central_dir_header_t *out) {
    errno = 0;
    int bytes = read(fd, out, sizeof(zip_central_dir_header_t));
    if (errno != 0) {
        return false;
    }
    if (bytes != sizeof(zip_central_dir_header_t)) {
        errno = EOF;
        return false;
    }

    return true;
}

bool zip_read_end_central_dir(int fd, zip_end_central_dir_header_t *out) {
    // save position
    off_t orig_pos = lseek(fd, 0, SEEK_CUR);

    // rewind to central record from EOF
    off_t length = lseek(fd, 0, SEEK_END);
    off_t end_search = EODC_COMMENT_MAX + (sizeof(zip_end_central_dir_header_t) + sizeof(zip_header_signature_t));
    if (length > end_search) {
        end_search = length - end_search;
    } else {
        end_search = 0;
    }
    lseek(fd, -4, SEEK_CUR);

    off_t pos = length - 4;

    zip_header_signature_t header;

    do {
        if (zip_read_signature(fd, &header) && header.type == HEADER_END_CENTRAL_DIR && header.magic == HEADER_MAGIC) {
            break;
        }

        // rewind 1 byte at a time
        pos = lseek(fd, -(sizeof(zip_header_signature_t) + 1), SEEK_CUR);
    } while (pos >= end_search);

    if (header.type != HEADER_END_CENTRAL_DIR || header.magic != HEADER_MAGIC) {
        errno = EINVAL;
        return false;
    }

    int bytes = read(fd, out, sizeof(zip_end_central_dir_header_t));
    if (bytes != sizeof(zip_end_central_dir_header_t)) {
        errno = EOF;
        return false;
    }

    // restore position
    lseek(fd, orig_pos, SEEK_SET);

    return true;
}

bool zip_read_data_descriptor(int fd, zip_data_descriptor_t *out) {
    int bytes = read(fd, out, sizeof(zip_data_descriptor_t));
    if (bytes != sizeof(zip_data_descriptor_t)) {
        errno = EOF;
        return false;
    }

    return true;
}