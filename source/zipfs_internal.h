#ifndef _ZIP_FS_INTERNAL_H_
#define _ZIP_FS_INTERNAL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/iosupport.h>

#include "zip/headers.h"

typedef struct zipfs_mount_point zipfs_mount_point_t;
typedef struct zipfs_file zipfs_file_t;
typedef struct zipfs_dirstate zipfs_dirstate_t;
typedef struct zipfs_mount_list zipfs_mount_list_t;

struct zipfs_mount_point {
    devoptab_t devoptab;
    int dev_id;
    zipfs_mount_list_t *list_item;

    int fd;

    zip_end_central_dir_header_t central_dir;
};

struct zipfs_file {
    int fd;
    uint32_t zip_offset;
    uint32_t position;
    uint32_t filesize;
    bool is_dir;
};

struct zipfs_dirstate {
};

struct zipfs_mount_list {
    zipfs_mount_point_t *mount;

    struct zipfs_mount_list *prev;
    struct zipfs_mount_list *next;
};

zipfs_mount_point_t *zipfs_get_mount(const char *path);

int zipfs_open(struct _reent *r, void *file_struct, const char *path, int flags, int mode);
int zipfs_close(struct _reent *r, int fd);
ssize_t zipfs_read(struct _reent *r, int fd, char *ptr, size_t len);
off_t zipfs_seek(struct _reent *r, int fd, off_t pos, int dir);
int zipfs_fstat(struct _reent *r, int fd, struct stat *st);
int zipfs_stat(struct _reent *r, const char *path, struct stat *st);

DIR_ITER *zipfs_diropen(struct _reent *r, DIR_ITER *dir_state, const char *path);
int zipfs_dirreset(struct _reent *r, DIR_ITER *dir_state);
int zipfs_dirnext(struct _reent *r, DIR_ITER *dir_state, char *filename, struct stat *filestat);
int zipfs_dirclose(struct _reent *r, DIR_ITER *dir_state);
int zipfs_statvfs(struct _reent *r, const char *path, struct statvfs *buf);

#endif // _ZIP_FS_INTERNAL_H_