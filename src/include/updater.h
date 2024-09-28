/**
 * @file
 * Header file for the updater GUI.
 */

#ifndef UPDATER_H
#define UPDATER_H

/**
 * URL where the updater will check for updates. "&version=VERSION" will
 * be appended to this URL, replacing VERSION with the current client's
 * version number.
 */
#define UPDATER_CHECK_URL "https://deusmagi.com/page/client_update"
/**
 * Base directory of all the updates. This is where the updates will be
 * downloaded from, as the updater server will only tell us the
 * filenames.
 */
#define UPDATER_PATH_URL "https://deusmagi.com/cms/uploads"

/**
 * A single update file that is to be downloaded.
 */
typedef struct update_file_struct {
    /** File name to download. */
    char *filename;

    /** SHA-1. */
    char *sha1;
} update_file_struct;

#endif
