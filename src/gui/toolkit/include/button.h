/**
 * @file
 * Button header file.
 */

#ifndef BUTTON_H
#define BUTTON_H

/** Determine whether the x,y position is over the specified button. */
#define BUTTON_MOUSE_OVER(button, mx, my, texture) ((mx) - (button)->px >= (button)->x && (mx) - (button)->px < (button)->x + (texture)->w && (my) - (button)->py >= (button)->y && (my) - (button)->py < (button)->y + (texture)->h)
/**
 * Checks if a tooltip can be generated for the specified button.
 */
#define BUTTON_CHECK_TOOLTIP(button) \
    ((button)->mouse_over && !(button)->pressed)

/** Button structure. */
typedef struct button_struct {
    /**
     * Surface to use for rendering.
     */
    SDL_Surface *surface;

    /** X position. */
    int x;

    /** Y position. */
    int y;

    int px;

    int py;

    /**
     * Texture to normally use for the button.
     */
    texture_struct *texture;

    /**
     * Texture to use if the mouse is over the button, NULL to use regular
     * one.
     */
    texture_struct *texture_over;

    /**
     * Texture to use if the button is being pressed, NULL to use regular
     * one.
     */
    texture_struct *texture_pressed;

    /** Font used for the text. */
    font_struct *font;

    /** Text flags. */
    uint64_t flags;

    /**
     * Whether to center the text vertically and horizontally.
     */
    uint8_t center;

    /** Color of the text. */
    const char *color;

    /** Color of the text's shadow. */
    const char *color_shadow;

    /** Color of the text if the mouse is over the button. */
    const char *color_over;

    /** Color of the text's shadow if the mouse is over the button. */
    const char *color_over_shadow;

    /** 1 if the mouse is over the button. */
    int mouse_over;

    /**
     * 1 if the button is being pressed.
     * @private
     */
    int pressed;

    /** 1 if the button should be forced to be pressed. */
    int pressed_forced;

    /** If 1, the button is in disabled state and cannot be pressed. */
    int disabled;

    /** When the button was pressed. */
    uint32_t pressed_ticks;

    /** When the mouse started hovering over the button. */
    uint32_t hover_ticks;

    /** Ticks needed to trigger a repeat. */
    uint32_t pressed_repeat_ticks;

    /**
     * Function called on button repeat
     * @param button
 * The button.
 */
    void (*repeat_func)(struct button_struct *button);

    /**
     * Whether the button needs redrawing.
     */
    uint8_t redraw;
} button_struct;

#endif
