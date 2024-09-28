/**
 * @file
 * Implements label type widgets.
 *
 * @author Alex Tokar
 * @author Daniel Liptrot
 */

#include <global.h>

/** @copydoc widgetdata::draw_func */
static void widget_draw(widgetdata *widget)
{
    _widget_label *label;

    label = widget->subwidget;

    if (label->text) {
        text_show(ScreenSurface, label->font, label->text, widget->x, widget->y, label->color, TEXT_MARKUP, NULL);
    }
}

/** @copydoc widgetdata::deinit_func */
static void widget_deinit(widgetdata *widget)
{
    _widget_label *label;

    label = widget->subwidget;

    if (label->text) {
        efree(label->text);
    }

    font_free(label->font);
}

/**
 * Initialize one label widget.
 */
void widget_label_init(widgetdata *widget)
{
    _widget_label *label;

    label = ecalloc(1, sizeof(*label));
    label->font = font_get("arial", 10);
    label->color = COLOR_WHITE;

    widget->draw_func = widget_draw;
    widget->deinit_func = widget_deinit;
    widget->subwidget = label;
}
