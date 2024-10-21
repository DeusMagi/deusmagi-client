/**
 * @file
 * This file controls various event functions, like character mouse movement,
 * parsing macro keys etc.
 */

#include <global.h>

/** @copydoc event_drag_cb_fnc */
static event_drag_cb_fnc event_drag_cb = NULL;

static int dragging_old_mx = -1, dragging_old_my = -1;

int event_dragging_check(void)
{
    int mx, my;
    float lx, ly;

    if (!cpl.dragging_tag) {
        return 0;
    }

    SDL_GetMouseState(&mx, &my);
    
    // map the mouse coordinates to our logical render size
    SDL_RenderWindowToLogical(ScreenRenderer, mx, my, &lx, &ly);
    mx = (int) lx;
    my = (int) ly;

    if (abs(cpl.dragging_startx - mx) < 3 && abs(cpl.dragging_starty - my) < 3) {
        return 0;
    }

    return 1;
}

int event_dragging_need_redraw(void)
{
    int mx, my;
    float lx, ly;

    if (!event_dragging_check()) {
        return 0;
    }

    SDL_GetMouseState(&mx, &my);
    
    // map the mouse coordinates to our logical render size
    SDL_RenderWindowToLogical(ScreenRenderer, mx, my, &lx, &ly);
    mx = (int) lx;
    my = (int) ly;

    if (mx != dragging_old_mx || my != dragging_old_my) {
        dragging_old_mx = mx;
        dragging_old_my = my;

        return 1;
    }

    return 0;
}

void event_dragging_start(tag_t tag, int mx, int my)
{
    float lx, ly;
    
    // map the mouse coordinates to our logical render size
    SDL_RenderWindowToLogical(ScreenRenderer, mx, my, &lx, &ly);
    mx = (int) lx;
    my = (int) ly;
    
    dragging_old_mx = -1;
    dragging_old_my = -1;

    cpl.dragging_tag = tag;
    cpl.dragging_startx = mx;
    cpl.dragging_starty = my;

    event_dragging_set_callback(NULL);
}

void event_dragging_set_callback(event_drag_cb_fnc fnc)
{
    event_drag_cb = fnc;
}

void event_dragging_stop(void)
{
    cpl.dragging_tag = 0;
}

static void event_dragging_stop_internal(void)
{
    if (event_dragging_check() && event_drag_cb != NULL) {
        event_drag_cb();
    }

    event_dragging_stop();
}

/**
 * Sets new width/height of the screen, storing the size in options.
 *
 * Does not actually do the resizing.
 * @param width
 * Width to set.
 * @param height
 * Height to set.
 */
void resize_window(int width, int height)
{
    setting_set_int(OPT_CAT_CLIENT, OPT_RESOLUTION_X, width);
    setting_set_int(OPT_CAT_CLIENT, OPT_RESOLUTION_Y, height);

    if (!setting_get_int(OPT_CAT_CLIENT, OPT_OFFSCREEN_WIDGETS) && width > 100 && height > 100) {
        widgets_ensure_onscreen();
    }
}

/**
 * Poll input device like mouse, keys, etc.
 * @return
 * 1 if the the quit key was pressed, 0 otherwise
 */
int Event_PollInputDevice(void)
{
    SDL_Event event;
    int x, y, done = 0;
    static Uint32 Ticks = 0;
    SDL_Keycode key;

    /* Execute mouse actions, even if mouse button is being held. */
    if ((SDL_GetTicks() - Ticks > 125) || !Ticks) {
        if (cpl.state >= ST_PLAY) {
            /* Mouse gesture: hold right+left buttons or middle button
             * to fire. */
            if (widget_mouse_event.owner == cur_widget[MAP_ID]) {
                if (map_mouse_fire()) {
                    Ticks = SDL_GetTicks();
                }
            }
        }
    }

    while (SDL_PollEvent(&event)) {
        x = event.motion.x;
        y = event.motion.y;

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                if (!keys[event.key.keysym.scancode].pressed) {
                    keys[event.key.keysym.scancode].repeated = 0;
                    keys[event.key.keysym.scancode].pressed = 1;
                    keys[event.key.keysym.scancode].time = LastTick + KEY_REPEAT_TIME_INIT;
                }
            }
        } else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                keys[event.key.keysym.scancode].pressed = 0;
            }
        } else if (event.type == SDL_MOUSEMOTION) {
            tooltip_dismiss();
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_PRINTSCREEN) {
            screenshot_create(ScreenSurface);
            continue;
        }

        switch (event.type) {
            /* Screen has been resized, update screen size. */
        case SDL_WINDOWEVENT_RESIZED:
            SDL_SetWindowSize(ScreenWindow, event.window.data1, event.window.data2);

            /* Set resolution to custom. */
            setting_set_int(OPT_CAT_CLIENT, OPT_RESOLUTION, 0);
            resize_window(event.window.data1, event.window.data2);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
        case SDL_KEYUP:
        case SDL_KEYDOWN:

            if (event.type == SDL_MOUSEMOTION) {
                cursor_x = x;
                cursor_y = y;
                
                SDL_SetCursor(system_cursor_arrow);
                cursor_texture = texture_get(TEXTURE_TYPE_CLIENT, "cursor_default");
            }

            if (popup_handle_event(&event)) {
                break;
            }

            if (event_dragging_check() && event.type != SDL_MOUSEBUTTONUP) {
                break;
            }

            if (cpl.state <= ST_WAITFORPLAY && intro_event(&event)) {
                break;
            } else if (cpl.state == ST_PLAY && widgets_event(&event)) {
                break;
            }

            if (cpl.state == ST_PLAY && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)) {
                key_handle_event(&event.key);
                break;
            }

            break;

        case SDL_QUIT:
            done = 1;
            break;

        default:
            break;
        }

        if (event.type == SDL_MOUSEBUTTONUP) {
            event_dragging_stop_internal();
        }
    }

    for (key = 0; key < SDL_NUM_SCANCODES; key++) {
        /* Ignore modifier keys. */
        if (KEY_IS_MODIFIER(key)) {
            continue;
        }

        if (keys[key].pressed && keys[key].time + KEY_REPEAT_TIME - 5 < LastTick) {
            keys[key].time = LastTick + KEY_REPEAT_TIME - 5;
            keys[key].repeated = 1;
            event_push_key(SDL_KEYDOWN, key, SDL_GetModState());
        }
    }

    return done;
}

void event_push_key(SDL_EventType type, SDL_Keycode key, SDL_Keymod mod)
{
    SDL_Event event;

    event.type = type;
    event.key.state = type == SDL_KEYDOWN ? SDL_PRESSED : SDL_RELEASED;
    event.key.keysym.sym = key;
    event.key.keysym.mod = mod;
    SDL_PushEvent(&event);
}

void event_push_key_once(SDL_Keycode key, SDL_Keymod mod)
{
    event_push_key(SDL_KEYDOWN, key, mod);
    event_push_key(SDL_KEYUP, key, mod);
}
