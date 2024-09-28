/**
 * @file
 * Sound related header file.
 */

#ifndef SOUND_H
#define SOUND_H

/**
 * @defgroup SOUND_TYPE_xxx Sound mixer type
 * Sound mixer types.
 *@{*/
/** Sound chunk, OGG/WAV, no MIDI. */
#define SOUND_TYPE_CHUNK 1
/** Music, OGG/MIDI/etc. */
#define SOUND_TYPE_MUSIC 2
/*@}*/

/**
 * One 'cached' sound.
 */
typedef struct sound_data_struct {
    /** The sound's data. */
    void *data;

    /** Sound's type, one of @ref SOUND_TYPE_xxx. */
    int type;

    /** Filename that was used to load sound_data_struct::data from. */
    char *filename;

    /** Hash handle. */
    UT_hash_handle hh;
} sound_data_struct;

#define POW2(x) ((x) * (x))

/** This value is defined in server too - change only both at once */
#define MAX_SOUND_DISTANCE 12

/**
 * One ambient sound effect.
 */
typedef struct sound_ambient_struct {
    /** Next ambient sound effect in a doubly-linked list. */
    struct sound_ambient_struct *next;

    /** Previous ambient sound effect in a doubly-linked list. */
    struct sound_ambient_struct *prev;

    /** ID of the object the sound is coming from. */
    int tag;

    /** Channel ID we are playing the sound effect on. */
    int channel;

    /** X position of the sound effect object on the client map. */
    int x;

    /** Y position of the sound effect object on the client map. */
    int y;

    /** Maximum range. */
    uint8_t max_range;
} sound_ambient_struct;

#endif
