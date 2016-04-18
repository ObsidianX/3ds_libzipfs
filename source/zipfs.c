#include <assert.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <sys/iosupport.h>
#include <unistd.h>

#include "../include/zipfs.h"
#include "zipfs_internal.h"

static devoptab_t _template_devoptab = {
        .name = NULL,
        .structSize = sizeof(zipfs_file_t),
        .dirStateSize = sizeof(zipfs_dirstate_t),

        .open_r = zipfs_open,
        .close_r = zipfs_close,
        .write_r = NULL,
        .read_r = zipfs_read,
        .seek_r = zipfs_seek,
        .fstat_r = zipfs_fstat,
        .stat_r = zipfs_stat,
        .link_r = NULL,
        .unlink_r = NULL,
        .chdir_r = NULL,
        .rename_r = NULL,
        .mkdir_r = NULL,

        .diropen_r = NULL,
        .dirreset_r = NULL,
        .dirnext_r = NULL,
        .dirclose_r = NULL,
        .statvfs_r = NULL,
        .ftruncate_r = NULL,
        .fsync_r = NULL,

        .chmod_r = NULL,
        .fchmod_r = NULL,
        .rmdir_r = NULL,
};

static zipfs_mount_list_t *_mount_list_head;
static zipfs_mount_list_t *_mount_list_tail;

zipfs_handle_t zipfs_mount(const char *filename, const char *mountpoint) {
    // read-only
    int fd = open(filename, O_RDONLY);

    zip_end_central_dir_header_t central_dir;
    if (!zip_read_end_central_dir(fd, &central_dir)) {
        close(fd);
        return NULL;
    }

    zipfs_mount_point_t *mount = malloc(sizeof(zipfs_mount_point_t));
    assert(mount != NULL);

    // copy defaults
    mount->devoptab = _template_devoptab;
    // copy mountpoint
    mount->devoptab.name = strdup(mountpoint);

    mount->fd = fd;
    mount->central_dir = central_dir;

    // mount
    mount->dev_id = AddDevice(&mount->devoptab);

    if (_mount_list_tail == NULL) {
        _mount_list_tail = malloc(sizeof(zipfs_mount_list_t));

        mount->list_item = _mount_list_tail;

        _mount_list_tail->mount = mount;
        _mount_list_tail->prev = NULL;
        _mount_list_tail->next = NULL;

        _mount_list_head = _mount_list_tail;
    } else {
        zipfs_mount_list_t *next = malloc(sizeof(zipfs_mount_list_t));

        mount->list_item = next;

        next->mount = mount;
        next->prev = _mount_list_tail;
        next->next = NULL;

        _mount_list_tail->next = next;
        _mount_list_tail = _mount_list_tail->next;
    }

    return mount;
}

void zipfs_set_default(zipfs_handle_t handle) {
    zipfs_mount_point_t *mount = handle;

    setDefaultDevice(mount->dev_id);
}

zipfs_mount_point_t *zipfs_get_mount(const char *path) {
    int dev = FindDevice(path);
    if (dev == -1) {
        return NULL;
    }

    zipfs_mount_list_t *item = _mount_list_head;
    while (item != NULL) {
        if (item->mount->dev_id == dev) {
            return item->mount;
        }

        item = item->next;
    }

    return NULL;
}

void zipfs_unmount(zipfs_handle_t handle) {
    zipfs_mount_point_t *mount = handle;

    RemoveDevice(mount->devoptab.name);

    zipfs_mount_list_t *list_item = mount->list_item;

    if (list_item->prev != NULL) {
        list_item->prev->next = list_item->next;
    } else {
        _mount_list_head = list_item->next;
    }
    if (list_item->next != NULL) {
        list_item->next->prev = list_item->prev;
    } else {
        _mount_list_tail = list_item->prev;
    }

    free(mount->list_item);
    free((void *) mount->devoptab.name);
    close(mount->fd);

    free(mount);
}