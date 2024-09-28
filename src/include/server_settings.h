/**
 * @file
 * Header file for server settings information.
 */

#ifndef SERVER_SETTINGS_H
#define SERVER_SETTINGS_H

/** Text IDs from server_settings file. */
enum {
    SERVER_TEXT_PROTECTION_GROUPS,
    SERVER_TEXT_PROTECTION_LETTERS,
    SERVER_TEXT_PROTECTION_FULL,
    SERVER_TEXT_SPELL_PATHS,
    SERVER_TEXT_ALLOWED_CHARS_ACCOUNT,
    SERVER_TEXT_ALLOWED_CHARS_ACCOUNT_MAX,
    SERVER_TEXT_ALLOWED_CHARS_CHARNAME,
    SERVER_TEXT_ALLOWED_CHARS_CHARNAME_MAX,
    SERVER_TEXT_ALLOWED_CHARS_PASSWORD,
    SERVER_TEXT_ALLOWED_CHARS_PASSWORD_MAX,

    SERVER_TEXT_MAX
};

/** One character. */
typedef struct char_struct {
    /** The race name. */
    char *name;

    /** Archetypes of the race's genders. */
    char *gender_archetypes[GENDER_MAX];

    /** Face names of the race's genders. */
    char *gender_faces[GENDER_MAX];

    /** Description of the race. */
    char *desc;
} char_struct;

/**
 * Server settings structure, initialized from the server_settings srv
 * file.
 */
typedef struct server_settings {
    /** Maximum reachable level. */
    uint8_t max_level;

    /** Experience needed for each level. */
    int64_t *level_exp;

    /** Races that can be selected to be played. */
    char_struct *characters;

    /** Number of server_settings::characters. */
    size_t num_characters;

    /** Server-configured strings. */
    char *text[SERVER_TEXT_MAX];

    /** Protection group names. */
    char *protection_groups[5];

    /** Protection few-letter acronyms. */
    char *protection_letters[20];

    /** Full protection names. */
    char *protection_full[20];

    /** Spell path names. */
    char *spell_paths[SPELL_PATH_NUM];
} server_settings;

#endif
