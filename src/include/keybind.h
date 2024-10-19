/**
 * @file
 * Keybindings header file.
 */

#ifndef KEYBIND_H
#define KEYBIND_H

/** Where keybindings are saved. */
#define FILE_KEYBIND "settings/keys.dat"

/**
 * One keybind.
 */
typedef struct keybind_struct {
    /** Command to execute. */
    char *command;

    /** Key bound. */
    SDL_Scancode key;

    /** Ctrl/shift/etc modifiers. */
    SDL_Keymod mod;

    /** Whether to trigger repeat. */
    uint8_t repeat;
} keybind_struct;

/** How quickly the key repeats. */
#define KEY_REPEAT_TIME (35)
/** Ticks that must pass before the key begins repeating. */
#define KEY_REPEAT_TIME_INIT (175)
/** Check whether the specified key is a modifier key. */
#define KEY_IS_MODIFIER(_key) ((_key) >= SDL_SCANCODE_NUMLOCKCLEAR)

#endif
