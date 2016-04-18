#ifndef _ZIP_FS_H_
#define _ZIP_FS_H_

typedef void *zipfs_handle_t;

zipfs_handle_t zipfs_mount(const char *filename, const char *mountpoint);
void zipfs_set_default(zipfs_handle_t handle);
void zipfs_unmount(zipfs_handle_t handle);

#endif // _ZIP_FS_H_