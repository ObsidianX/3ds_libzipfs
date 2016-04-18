#ifndef _ZIP_H_
#define _ZIP_H_

#include <stdbool.h>
#include <stdint.h>

#include "headers.h"

#define CRC_MAGIC 0xDEBB20E3

// @formatter:off
enum {
    // Implode flags
    FLAG_IMPLODE_8K         = 1 << 0,
    FLAG_IMPLODE_3_TREES    = 1 << 1,
    // LZMA flags
    FLAG_LZMA_USE_EOS       = 1 << 0,

    // Standard flags
    FLAG_ENCRYPTED          = 0,
    FLAG_SEPARATE_DATA_DESC = 1 << 2,
    FLAG_COMPRESSED_PATCHED = 1 << 4,
    FLAG_STRONG_ENCRYPTION  = 1 << 5,
    FLAG_UTF8               = 1 << 10,
    FLAG_ENCRYPTED_CENTRAL  = 1 << 12,
};
// @formatter:on

/**
 * Attempts to find a central directory entry for the given absolute path
 */
bool zip_find_path(int fd, zip_end_central_dir_header_t *central_dir, const char *path, zip_central_dir_header_t *out);

#endif // _ZIP_H_