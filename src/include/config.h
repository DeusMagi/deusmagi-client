/**
 * @file
 * This is the config file for the client.
 */

#ifndef CONFIG_H
#define CONFIG_H

/** Socket version. */
#define SOCKET_VERSION 1066

/** File the the arch definitions. */
#define ARCHDEF_FILE "data/archdef.dat"
/** File with the widgets' positions. */
#define INTERFACE_FILE "settings/interface.gui"
/** What scripts to autoload on client startup. */
#define SCRIPTS_AUTOLOAD "settings/scripts_autoload"
/** Log file. */
#define LOG_FILE "client.log"
/** Name of the icon. */
#define CLIENT_ICON_NAME "icon.png"
/** File that contains the default settings. */
#define FILE_SETTINGS_TXT "data/settings.txt"
/** File that contains the user settings. */
#define FILE_SETTINGS_DAT "settings/settings.dat"

/** Maximum number of faces. */
#define MAX_FACE_TILES 32767

/** Maximum map size. */
#define MAP_MAX_SIZE 17

/**
 * Size of the Fog of War cache. Setting this to 1 will decrease memory usage
 * somewhat, but make Fog of War less useful when moving across maps.
 *
 * Basically this value represents how big grid of maps to keep in memory. For
 * example, if the value is 3, the grid will be 3x3, which means 9 "maps", and
 * the middle one is the currently displayed one.
 */
#define MAP_FOW_SIZE 5

/**
 * The number of our dark levels.
 *
 * For each level we store an own bitmap copy.
 */
#define DARK_LEVELS 7

#define DIRECTORY_SFX "sounds/effects"
#define DIRECTORY_CACHE "cache"
#define DIRECTORY_FACES "faces"
#define DIRECTORY_MEDIA "sounds/background"

/**
 * This is the minimum server version required to connect, represented as
 * hexadecimal and backwards. Thus, 0x0503 requires at least 3.5, and 0x010503
 * requires at least 3.5.1.
 */
#define SERVER_VERSION 0x04

#endif
