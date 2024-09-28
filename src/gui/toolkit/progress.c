/**
 * @file
 * Implements progress indicators.
 *
 * @author Alex Tokar
 */

#include <global.h>

/**
 * Create progress dots indicator.
 * @param progress
 * Where to store indicator data.
 */
void progress_dots_create(progress_dots *progress)
{
    progress->ticks = SDL_GetTicks();
    progress->dot = 0;
    progress->done = 0;
}

/**
 * Show the progress dots.
 * @param progress
 * The progress dots indicator data.
 * @param surface
 * Surface to draw on.
 * @param x
 * X position.
 * @param y
 * Y position.
 */
void progress_dots_show(progress_dots *progress, SDL_Surface *surface, int x, int y)
{
    uint8_t i;
    SDL_Surface *texture;

    for (i = 0; i < PROGRESS_DOTS_NUM; i++) {
        texture = TEXTURE_CLIENT(progress->dot == i || progress->done ? "loading_on" : "loading_off");
        surface_show(surface, x + (texture->w + PROGRESS_DOTS_SPACING) * i, y, NULL, texture);
    }

    /* Progress the lit dot. */
    if (!progress->done && SDL_GetTicks() - progress->ticks > PROGRESS_DOTS_TICKS) {
        progress->ticks = SDL_GetTicks();
        progress->dot++;

        /* More than maximum, back to the first one. */
        if (progress->dot >= PROGRESS_DOTS_NUM) {
            progress->dot = 0;
        }
    }
}

/**
 * Calculate the width of progress dots.
 * @param progress
 * The progress dots indicator data.
 * @return
 * The width.
 */
int progress_dots_width(progress_dots *progress)
{
    (void) progress;

    return (TEXTURE_CLIENT("loading_on")->w + PROGRESS_DOTS_SPACING) * PROGRESS_DOTS_NUM - PROGRESS_DOTS_SPACING;
}
