/**
 * @file
 * Implements the credits popup.
 *
 * @author Alex Tokar
 */

#include <global.h>

#define CREDITS_WIDTH 455
#define CREDITS_HEIGHT 250
#define CREDITS_SCROLL_TICKS 80

/**
 * The contributors help file.
 */
static hfile_struct *hfile_contributors;
/**
 * Scroll value of the credits.
 */
static int credits_scroll;
/**
 * Height of the credits text.
 */
static int credits_height;
/**
 * Last scrolled ticks.
 */
static uint32_t credits_ticks;

/** @copydoc popup_struct::draw_func */
static int popup_draw_func(popup_struct *popup)
{
    SDL_Rect box;

    box.w = popup->surface->w;
    box.h = 38;
    text_show(popup->surface, FONT_SERIF16, "Game Credits", 0, 0, COLOR_HGOLD, TEXT_ALIGN_CENTER | TEXT_VALIGN_CENTER, &box);

    box.x = 0;
    box.y = credits_scroll;
    box.w = CREDITS_WIDTH;
    box.h = CREDITS_HEIGHT;
    text_show(popup->surface, FONT_ARIAL11, hfile_contributors->msg, 10, 40, COLOR_WHITE, TEXT_WORD_WRAP | TEXT_MARKUP | TEXT_HEIGHT, &box);

    if (SDL_GetTicks() - credits_ticks > CREDITS_SCROLL_TICKS) {
        credits_scroll++;
        credits_ticks = SDL_GetTicks();
    }

    return credits_scroll < credits_height;
}

/**
 * Show the credits popup.
 */
void credits_show(void)
{
    SDL_Rect box;
    popup_struct *popup;

    hfile_contributors = help_find("contributors");

    if (!hfile_contributors) {
        LOG(BUG, "Could not find 'contributors' help file.");
        return;
    }

    credits_scroll = 0;
    credits_ticks = SDL_GetTicks();

    /* Calculate the height. */
    box.w = CREDITS_WIDTH;
    box.h = CREDITS_HEIGHT;
    text_show(NULL, FONT_ARIAL11, hfile_contributors->msg, 0, 0, COLOR_BLACK, TEXT_WORD_WRAP | TEXT_MARKUP | TEXT_HEIGHT, &box);
    credits_height = box.h;

    popup = popup_create(texture_get(TEXTURE_TYPE_CLIENT, "popup"));
    popup->draw_func = popup_draw_func;
}
