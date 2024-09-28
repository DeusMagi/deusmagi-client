/**
 * @file
 * Implements FPS type widgets.
 *
 * @author Alex Tokar
 */

#include <global.h>

/**
 * FPS widget data.
 */
typedef struct widget_fps_struct {
    /**
     * Last time the FPS was calculated.
     */
    uint32_t lasttime;

    /**
     * Current FPS.
     */
    uint32_t current;

    /**
     * Real number of frames rendered in the last second.
     */
    uint32_t current_real;

    /**
     * Number of main loop iterations since last calculation.
     */
    uint32_t frames;

    /**
     * Real number of frames drawn since last calculation.
     */
    uint32_t frames_real;
} widget_fps_struct;

/** @copydoc widgetdata::draw_func */
static void widget_draw(widgetdata *widget)
{
    widget_fps_struct *tmp;

    if (!widget->redraw) {
        return;
    }

    tmp = widget->subwidget;

    text_show_format(widget->surface, FONT_ARIAL11, 4, 4, COLOR_WHITE, 0, NULL,
            "%d (%d)", tmp->current, tmp->current_real);
}

/** @copydoc widgetdata::background_func */
static void widget_background(widgetdata *widget, int draw)
{
    widget_fps_struct *tmp;
    uint32_t ticks;

    tmp = widget->subwidget;
    tmp->frames++;
    tmp->frames_real += draw;
    ticks = SDL_GetTicks();

    if (tmp->lasttime < ticks - 1000) {
        if (tmp->current != tmp->frames ||
                tmp->current_real != tmp->frames_real) {
            widget->redraw = 1;
        }

        tmp->lasttime = ticks;
        tmp->current = tmp->frames;
        tmp->current_real = tmp->frames_real;
        tmp->frames = 0;
        tmp->frames_real = 0;
    }
}

/**
 * Initialize one FPS widget.
 */
void widget_fps_init(widgetdata *widget)
{
    widget_fps_struct *tmp;

    widget->draw_func = widget_draw;
    widget->background_func = widget_background;

    widget->subwidget = tmp = ecalloc(1, sizeof(*tmp));
    tmp->lasttime = SDL_GetTicks();
}
