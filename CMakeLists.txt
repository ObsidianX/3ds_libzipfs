cmake_minimum_required(VERSION 3.4)
project(libzipfs)

include_directories(/opt/devkitpro/devkitARM/arm-none-eabi/include)
include_directories(/opt/devkitpro/libctru/include)

set(HEADER_FILES
        include/zipfs.h
        source/zipfs_internal.h
        source/zip/zip.h
        source/zip/headers.h)

set(SOURCE_FILES
        ${HEADER_FILES}
        main.c
        source/zipfs.c
        source/close.c
        source/dirclose.c
        source/dirnext.c
        source/diropen.c
        source/dirreset.c
        source/fstat.c
        source/open.c
        source/read.c
        source/seek.c
        source/stat.c
        source/zip/zip.c
        source/zip/header.c)
add_executable(libzipfs ${SOURCE_FILES})