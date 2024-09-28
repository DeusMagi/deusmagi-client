/**
 * @file
 * Implements text input type widgets.
 *
 * @author Alex Tokar
 */

#include <global.h>
#include <toolkit/string.h>

static void widget_input_handle_enter(widgetdata *widget)
{
    text_input_struct *text_input;

    widget->show = 0;
    text_input = &WIDGET_INPUT(widget)->text_input;

    if (*text_input->str != '\0') {
        StringBuffer *sb;
        char *cp, *str;

        sb = stringbuffer_new();

        if (*text_input->str != '/') {
            stringbuffer_append_string(sb, WIDGET_INPUT(widget)->prepend_text);
        }

        str = text_escape_markup(text_input->str);
        stringbuffer_append_string(sb, str);
        efree(str);

        cp = stringbuffer_finish(sb);
        send_command_check(cp);
        efree(cp);
    }
}

/** @copydoc widgetdata::draw_func */
static void widget_draw(widgetdata *widget)
{
    text_input_struct *text_input;

    widget->redraw++;

    text_input = &WIDGET_INPUT(widget)->text_input;
    text_input->coords.w = widget->w - 16;
    text_input_show(text_input, widget->surface, widget->w / 2 - text_input->coords.w / 2, widget->h / 2 - text_input->coords.h / 2 + 8);

    text_truncate_overflow(FONT_ARIAL10, WIDGET_INPUT(widget)->title_text, 220);
    text_show(widget->surface, FONT_ARIAL10, WIDGET_INPUT(widget)->title_text, 8, 6, COLOR_HGOLD, 0, NULL);
}

/** @copydoc widgetdata::event_func */
static int widget_event(widgetdata *widget, SDL_Event *event)
{
    widget_input_struct *input;
    text_input_struct *text_input;

    if (widget->show && event->type == SDL_KEYDOWN) {
        input = WIDGET_INPUT(widget);

        if (SDL_GetTicks() - widget->showed_ticks > 125 && ((string_startswith(input->prepend_text, "/gettag ") && keybind_command_matches_event("?GET", &event->key)) || (string_startswith(input->prepend_text, "/droptag ") && keybind_command_matches_event("?DROP", &event->key)))) {
            widget_input_handle_enter(widget);
            keys[event->key.keysym.scancode].time = SDL_GetTicks() + 125;
            return 1;
        }

        text_input = &input->text_input;

        if (IS_ENTER(event->key.keysym.sym)) {
            widget_input_handle_enter(widget);
        }

        if (event->key.keysym.sym == SDLK_ESCAPE) {
            widget->show = 0;
            return 1;
        } else if (event->key.keysym.sym == SDLK_TAB) {
            help_handle_tabulator(text_input);
            return 1;
        } else if (text_input_event(text_input, event)) {
            return 1;
        }
    }

    return 0;
}

/** @copydoc widgetdata::deinit_func */
static void widget_deinit(widgetdata *widget)
{
    widget_input_struct *input;

    input = WIDGET_INPUT(widget);

    text_input_destroy(&input->text_input);
    text_input_history_free(input->text_input_history);
}

void widget_input_init(widgetdata *widget)
{
    widget_input_struct *input;

    input = ecalloc(1, sizeof(*input));
    text_input_create(&input->text_input);
    input->text_input.max = 250;
    input->text_input_history = text_input_history_create();

    widget->draw_func = widget_draw;
    widget->event_func = widget_event;
    widget->deinit_func = widget_deinit;
    widget->subwidget = input;
}
