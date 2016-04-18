#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "zipfs_internal.h"
#include "zip/zip.h"

int zipfs_open(struct _reent *r, void *file_struct, const char *path, int flags, int mode) {
    zipfs_file_t *file = (zipfs_file_t *) file_struct;

    if ((flags & O_ACCMODE) != O_RDONLY) {
        errno = EROFS;
        return -1;
    }

    zipfs_mount_point_t *mount = zipfs_get_mount(path);
    if (mount == NULL) {
        return -1;
    }

    char internal_path[FILENAME_MAX];

    char test_dev_name[FILENAME_MAX];
    sprintf(test_dev_name, "%s:/", mount->devoptab.name);
    size_t test_dev_name_len = strlen(test_dev_name);

    if (!strncmp(path, test_dev_name, test_dev_name_len)) {
        strcpy(internal_path, path + test_dev_name_len);
    } else if (!strncmp(path, "/", 1)) {
        strcpy(internal_path, path + 1);
    } else {
        strcpy(internal_path, path);
    }

    zip_central_dir_header_t header;
    if (!zip_find_path(mount->fd, &mount->central_dir, internal_path, &header)) {
        errno = EINVAL;
        return -1;
    }

    file->fd = mount->fd;
    file->position = 0;
    file->zip_offset = header.local_offset;
    file->filesize = header.data_descriptor.orig_size;
    file->is_dir = ZIP_IS_DIR(header.external_file_attrs);

    return 0;
}