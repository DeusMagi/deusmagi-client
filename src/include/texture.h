/**
 * @file
 * Texture system header file.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

/**
 * Texture types.
 */
typedef enum texture_type_t {
    TEXTURE_TYPE_SOFTWARE,
    TEXTURE_TYPE_CLIENT,

    TEXTURE_TYPE_NUM
} texture_type_t;

typedef struct texture_struct {
    char *name;

    texture_type_t type;

    time_t last_used;

    SDL_Surface *surface;

    UT_hash_handle hh;
} texture_struct;

#define TEXTURE_FALLBACK_NAME "texture_fallback"

#define TEXTURE_CLIENT(_name) (texture_surface(texture_get(TEXTURE_TYPE_CLIENT, (_name))))

#define TEXTURE_GC_MAX_TIME 100000
#define TEXTURE_GC_CHANCE 500
#define TEXTURE_GC_FREE_TIME 60 * 15

#endif
