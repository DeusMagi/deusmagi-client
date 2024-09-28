/**
 * @file
 * Party header file.
 */

#ifndef PARTY_H
#define PARTY_H

/**
 * @defgroup CMD_PARTY_xxx Party socket command types
 * Various types of the BINARY_CMD_PARTY socket command.
 *@{*/
/**
 * Show a list of all parties in the game.
 */
#define CMD_PARTY_LIST 1
/**
 * Show current members of your party.
 */
#define CMD_PARTY_WHO 2
/**
 * Successfully joined a party.
 */
#define CMD_PARTY_JOIN 3
/**
 * Joining a party requires a password.
 */
#define CMD_PARTY_PASSWORD 4
/**
 * We're leaving a party.
 */
#define CMD_PARTY_LEAVE 5
/**
 * Update party's who list.
 */
#define CMD_PARTY_UPDATE 6
/**
 * Remove memebr from party's who list.
 */
#define CMD_PARTY_REMOVE_MEMBER 7
/*@}*/

#endif
