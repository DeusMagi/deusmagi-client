/**
 * @file
 * Implements texture type widgets.
 *
 * @author Alex Tokar
 * @author Daniel Liptrot
 */

#include <global.h>

/** @copydoc widgetdata::draw_func */
static void widget_draw(widgetdata *widget)
{
    _widget_texture *texture;

    texture = widget->subwidget;

    if (texture->texture) {
        surface_show(ScreenSurface, widget->x, widget->y, NULL, texture_surface(texture->texture));
    }
}

/**
 * Initialize one texture widget.
 */
void widget_texture_init(widgetdata *widget)
{
    _widget_texture *texture;

    texture = ecalloc(1, sizeof(*texture));

    widget->draw_func = widget_draw;
    widget->subwidget = texture;
}
