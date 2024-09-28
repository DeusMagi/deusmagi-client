/**
 * @file
 * Image related structures.
 */

#ifndef IMAGE_H
#define IMAGE_H

/**
 * Structure for bmap data.
 */
typedef struct bmap {
    /**
     * The name.
     */
    char *name;

    /**
     * Size in bytes.
     */
    size_t len;

    /**
     * Position in game.p0 file, if -1, it doesn't exist in the file.
     */
    long pos;

    /**
     * Checksum.
     */
    unsigned long crc32;
} bmap_t;

/**
 * Container structure for bmaps.
 */
typedef struct bmap_hash {
    /**
     * Actual bmap data.
     */
    bmap_t bmap;

    /**
     * Hash handle.
     */
    UT_hash_handle hh;
} bmap_hash_t;

/* Prototypes */
void image_init(void);
void image_deinit(void);
void image_bmaps_init(void);
void image_bmaps_deinit(void);
void finish_face_cmd(int facenum, uint32_t checksum, const char *face);
void image_request_face(int pnum);
int image_get_id(const char *name);

#endif
