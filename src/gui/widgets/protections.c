/**
 * @file
 * Implements protections type widgets.
 *
 * @author Alex Tokar
 */

#include <global.h>
#include <toolkit/string.h>

/** @copydoc widgetdata::draw_func */
static void widget_draw(widgetdata *widget)
{
    size_t i, j;
    SDL_Rect box[4];
    char buf[HUGE_BUF];
    const char *color;

    if (!widget->redraw) {
        return;
    }

    for (i = 0; i < arraysize(cpl.stats.protection) / 5; i++) {
#define PAD_BORDER 10
#define PADX_BOX 10
#define PADY_BOX 5
        box[i].w = (widget->w - PAD_BORDER * 2 - PADX_BOX) / 2;
        box[i].h = (widget->h - PAD_BORDER * 2 - PADY_BOX) / 2;
        box[i].x = PAD_BORDER + (box[i].w + PADX_BOX) * ((i % 2));
        box[i].y = PAD_BORDER + (box[i].h + PADY_BOX) * (i / 2);
#undef PAD_BORDER
#undef PADX_BOX
#undef PADY_BOX

        snprintf(buf, sizeof(buf),
                "[o=#000000][center][font=serif][size=+3]%s[/size][/font]"
                "[/center][/o]", s_settings->protection_groups[i]);

        for (j = 0; j < 5; j++) {
            if (cpl.stats.protection[i * 5 + j] == 0) {
                color = COLOR_GRAY;
            } else if (cpl.stats.protection[i * 5 + j] < 0) {
                color = COLOR_RED;
            } else if (cpl.stats.protection[i * 5 + j] >= 100) {
                color = COLOR_ORANGE;
            } else {
                color = COLOR_WHITE;
            }

            snprintfcat(buf, sizeof(buf),
                    "\n%s[c=#%s][right][font=mono]%02d[/font][/right][/c]",
                    s_settings->protection_full[i * 5 + j], color,
                    cpl.stats.protection[i * 5 + j]);
        }

        text_show(widget->surface, FONT("serif", 11), buf, box[i].x, box[i].y,
                COLOR_HGOLD, TEXT_MARKUP, &box[i]);
    }
}

/**
 * Initialize one protections widget.
 */
void widget_protections_init(widgetdata *widget)
{
    widget->draw_func = widget_draw;
}
