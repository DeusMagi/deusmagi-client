/**
 * @file
 * Ignore list related header.
 */

#ifndef IGNORE_H
#define IGNORE_H

/** The ignore list structure. */
typedef struct _ignore_list {
    /** Next entry in this list. */
    struct _ignore_list *next;

    /** Name to ignore. */
    char name[MAX_BUF];

    /** Type to ignore. */
    char type[MAX_BUF];
} ignore_list_struct;

#endif
