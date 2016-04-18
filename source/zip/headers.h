#ifndef _ZIP_HEADERS_H_
#define _ZIP_HEADERS_H_

#include <stdbool.h>

#define HEADER_MAGIC 0x4B50

#define HEADER_CENTRAL_DIR      0x0201
#define HEADER_LOCAL_FILE       0x0403
#define HEADER_DIG_SIG          0x0505
#define HEADER_END_CENTRAL_DIR  0x0605
#define HEADER_Z64_CENTRAL_DIR  0x0606
#define HEADER_EXTRA            0x0806
#define HEADER_DATA_DESC        0x0807

#define MSDOS_DIR_ATTR (1 << 5)
#define ZIP_IS_DIR(attrs) ((attrs & MSDOS_DIR_ATTR) != 0)

// signature is read as a single, little endian 4-byte word
typedef union __packed {
    struct __packed {
        uint16_t magic;
        uint16_t type;
    };
    uint32_t raw;
} zip_header_signature_t;

typedef struct __packed {
    uint32_t crc32;
    uint32_t pack_size;
    uint32_t orig_size;
} zip_data_descriptor_t;

typedef struct __packed {
    uint32_t length;
    uint8_t *data;
} zip_extra_field_t;

typedef struct __packed {
    uint16_t version_extract;
    uint16_t flags;
    uint16_t compression;
    uint16_t mtime;
    uint16_t mdate;
    zip_data_descriptor_t data_descriptor;
    uint16_t filename_len;
    uint16_t extra_len;
} zip_local_file_header_t;

typedef struct __packed {
    uint16_t version_created;
    uint16_t version_extract;
    uint16_t flags;
    uint16_t compression;
    uint16_t mtime;
    uint16_t mdate;
    zip_data_descriptor_t data_descriptor;
    uint16_t filename_len;
    uint16_t extra_len;
    uint16_t comment_len;
    uint16_t disk_num_start;
    uint16_t internal_file_attrs;
    uint32_t external_file_attrs;
    uint32_t local_offset;
} zip_central_dir_header_t;

typedef struct __packed {
    uint16_t disk_num;
    uint16_t disk_central_start;
    uint16_t disk_records;
    uint16_t total_records;
    uint32_t total_size;
    uint32_t offset;
    uint16_t comment_len;
} zip_end_central_dir_header_t;

typedef struct __packed {
    uint16_t length;
    uint8_t *data;
} zip_signature_t;

/**
 * Reads the first 2 bytes of a ZIP entry representing the entry signature
 */
bool zip_read_signature(int fd, zip_header_signature_t *out);

/**
 * Reads the metadata for a Local File entry
 * Note: does not read the filename or extras
 */
bool zip_read_local_file(int fd, zip_local_file_header_t *out);

/**
 * Reads the metadata for a Central Directory entry
 * Note: does not read the filename, extras, or comment
 */
bool zip_read_central_dir(int fd, zip_central_dir_header_t *out);

/**
 * Finds and reads the End-of-Central-Directory entry at the end of the file
 * Note: does not read the comment
 * Note: restores file position after reading the entry
 */
bool zip_read_end_central_dir(int fd, zip_end_central_dir_header_t *out);

/**
 * Reads the data descriptor field sometimes found after other entries
 */
bool zip_read_data_descriptor(int fd, zip_data_descriptor_t *out);

#endif // _ZIP_HEADER_H_