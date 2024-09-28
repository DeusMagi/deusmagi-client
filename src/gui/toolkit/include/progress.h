/**
 * @file
 * Header file for progress indicators.
 */

#ifndef PROGRESS_H
#define PROGRESS_H

/**
 * Dots progress indicator.
 */
typedef struct progress_dots {
    /** Last time when one of the dots was lit. */
    uint32_t ticks;

    /** Which dot is currently lit. */
    uint8_t dot;

    /** Whether the progress is done. */
    uint8_t done;
} progress_dots;

/** Number of progress dots shown. */
#define PROGRESS_DOTS_NUM (5)
/** Spacing between the dots. */
#define PROGRESS_DOTS_SPACING (2)
/** How often to advance the progress dots, in ticks. */
#define PROGRESS_DOTS_TICKS (275)

#endif
