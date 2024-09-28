/**
 * @file
 * Header file for servers files declarations.
 */

#ifndef SERVER_FILES_H
#define SERVER_FILES_H

#include <toolkit/curl.h>

#define SERVER_FILES_HTTP_DIR       "data"
#define SERVER_FILES_HTTP_LISTING   "listing.txt"

#define SERVER_FILE_ANIMS           "anims"
#define SERVER_FILE_BMAPS           "bmaps"
#define SERVER_FILE_UPDATES         "updates"
#define SERVER_FILE_SETTINGS        "settings"
#define SERVER_FILE_EFFECTS         "effects"
#define SERVER_FILE_HFILES          "hfiles"

/** One server file. */
typedef struct server_files_struct {
    /** Name of the server file. */
    char *name;

    /** If 0, will be (re-)loaded. */
    uint8_t loaded;

    /**
     * Update status of this file:
     *
     * - 0: Not being updated, or just finished updating.
     * - 1: Start updating the file the next time server_files_updating()
     *      is called.
     * - -1: The file is being updated.
     */
    int8_t update;

    /** Size of the file. */
    size_t size;

    /** Calculated checksum. */
    unsigned long crc32;

    /** cURL request. */
    curl_request_t *request;

    /** Init-time function. */
    void (*init_func)(void);

    /** Function to call when re-loading. */
    void (*reload_func)(void);

    /** Loading/parsing function. */
    void (*parse_func)(void);

    /** Hash handle. */
    UT_hash_handle hh;
} server_files_struct;

#endif
