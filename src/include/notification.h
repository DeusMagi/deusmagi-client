/**
 * @file
 * Notification header file.
 */

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

/**
 * The notification data.
 */
typedef struct notification_struct {
    /** Current alpha value of the notification surface. */
    int alpha;

    /** Macro or command to execute when the notification is clicked. */
    char *action;

    /** Macro that is temporarily bound to the notification's action. */
    char *shortcut;

    /** When the notification was created. */
    uint32_t start_ticks;

    /**
     * Milliseconds that must pass before the notification is
     * dismissed.
     */
    uint32_t delay;
} notification_struct;

/**
 * @defgroup CMD_NOTIFICATION_xxx Notification command types
 * Notification command types.
 *@{*/
/** The notification contents. */
#define CMD_NOTIFICATION_TEXT 0
/** What macro or command to execute. */
#define CMD_NOTIFICATION_ACTION 1
/** Macro temporarily assigned to this notification. */
#define CMD_NOTIFICATION_SHORTCUT 2
/**
 * How many milliseconds must pass before the notification is
 * dismissed.
 */
#define CMD_NOTIFICATION_DELAY 3
/*@}*/

/**
 * @defgroup NOTIFICATION_DEFAULT_xxx Notification defaults
 * Default notification values.
 *@{*/
/** The maximum width of the notification. */
#define NOTIFICATION_DEFAULT_WIDTH 200
/** The font of the notification's message. */
#define NOTIFICATION_DEFAULT_FONT FONT_ARIAL11
/** Milliseconds before dismissal. */
#define NOTIFICATION_DEFAULT_DELAY (30 * 1000)
/** How many seconds before the dismissal to start the fadeout. */
#define NOTIFICATION_DEFAULT_FADEOUT (5 * 1000)
/*@}*/

#endif
