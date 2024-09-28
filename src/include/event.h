/**
 * @file
 * Event related header file.
 */

#ifndef EVENT_H
#define EVENT_H

enum {
    DRAG_GET_STATUS = -1,
    DRAG_NONE,
    DRAG_QUICKSLOT,
    DRAG_QUICKSLOT_SPELL
};

/**
 * Called when dragged object is not handled, and a handler was specified.
 */
typedef void (*event_drag_cb_fnc)(void);

/**
 * Key information.
 */
typedef struct key_struct {
    /** If 1, the key is pressed. */
    uint8_t pressed;

    /** Last repeat time. */
    uint32_t time;

    /** Whether the key is being repeated. */
    uint8_t repeated;
} key_struct;

#define EVENT_IS_MOUSE(_event) ((_event)->type == SDL_MOUSEBUTTONDOWN || (_event)->type == SDL_MOUSEBUTTONUP || (_event)->type == SDL_MOUSEMOTION)
#define EVENT_IS_KEY(_event) ((_event)->type == SDL_KEYDOWN || (_event)->type == SDL_KEYUP)

#endif
