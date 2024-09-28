/**
 * @file
 *
 */

#include <global.h>

key_struct keys[SDL_NUM_SCANCODES];

/**
 * Initialize keys.
 */
void init_keys(void)
{
    memset(keys, 0, sizeof(*keys) * arraysize(keys));
}

/**
 * Handle a keyboard event.
 * @param event
 * The event to handle.
 */
void key_handle_event(SDL_KeyboardEvent *event)
{
    if (event->type == SDL_KEYDOWN) {
        if (cpl.state == ST_PLAY && event->keysym.sym == SDLK_ESCAPE) {
            settings_open();
            return;
        }
    }

    keybind_process_event(event);
}
