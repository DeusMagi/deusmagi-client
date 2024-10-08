/**
 * @file
 * Header file for things that are generally used in many places.
 */

#ifndef MAIN_H
#define MAIN_H

#define COLOR_BUF 7

#define SDL_DEFAULT_REPEAT_INTERVAL 30

/* For hash table (bmap, ...) */
#define MAXSTRING 20

/**
 * Structure containing server certificate information.
 */
typedef struct server_cert_info {
    char *name; ///< Server name.
    char *hostname; ///< Server hostname.

    int port; ///< Regular port.
    int port_crypto; ///< Crypto port.

    char *ipv4_address; ///< IPv4 address. Can be NULL.
    char *ipv6_address; ///< IPv6 address. Can be NULL.

    char *pubkey; ///< Public key.
} server_cert_info_t;

/** The servers list, as given by the metaserver. */
typedef struct server_struct {
    /** Next server in the list. */
    struct server_struct *next;

    /** Previous server in the list. */
    struct server_struct *prev;

    /** Name of the server. */
    char *name;

    /** Hostname of the server. */
    char *hostname;

    /** Server version. */
    char *version;

    /** Server description. */
    char *desc;

    /** Number of players online. */
    int player;

    /** Server port. */
    int port;

    /** Crypto port. -1 if disabled. */
    int port_crypto;

    /** Certificate public key. */
    char *cert_pubkey;

    /** Certificate entry. */
    char *cert;

    /**
     * Certificate signature. The certificate is invalid without
     * a valid signature.
     */
    unsigned char *cert_sig;

    /**
     * Length of the certificate signature.
     */
    size_t cert_sig_len;

    /** Parsed certificate information. */
    server_cert_info_t *cert_info;

    /** Whether the entry was learned from the metaserver. */
    bool is_meta:1;
} server_struct;

/**
 * Acquire public key to use for the specified server.
 *
 * @param server
 * Server.
 * @return
 * Public key, NULL if there's no public key.
 * @todo
 * This should really be a function.
 */
#define METASERVER_GET_PUBKEY(server)                               \
    ((server)->cert_info != NULL ? (server)->cert_info->pubkey :    \
     (server)->cert_pubkey)

/**
 * Message animation structure. Used when NDI_ANIM is passed to
 * DrawInfoCmd2().
 */
typedef struct msg_anim_struct {
    /** The message to play. */
    char message[MAX_BUF];

    /** Tick when it started. */
    uint32_t tick;

    /** Color of the message animation. */
    char color[COLOR_BUF];
} msg_anim_struct;

#define FILE_GAME_P0 "data/game.p0"

/* Face requested from server - do it only one time */
#define FACE_REQUESTED      16

typedef struct _face_struct {
    /* Our face data. if != null, face is loaded */
    struct sprite_struct *sprite;

    /* Our face name. if != null, face is requested */
    char *name;

    /* Checksum of face */
    uint32_t checksum;

    int flags;
} _face_struct;

#define NUM_STATS 7

typedef struct spell_entry_struct {
    /**
     * The spell object in player's inventory.
     */
    object *spell;

    /**
     * Cost of spell.
     */
    uint16_t cost;

    /**
     * Spell's flags.
     */
    uint32_t flags;

    /**
     * Spell's path.
     */
    uint32_t path;

    /**
     * Description of the spell.
     */
    char msg[MAX_BUF];
} spell_entry_struct;

/**
 * Maximum number of spell paths. The last one is always 'all' and holds
 * pointers to spells in the other spell paths.
 */
#define SPELL_PATH_NUM 21

typedef struct skill_entry_struct {
    object *skill;

    uint8_t level;

    int64_t exp;

    char msg[MAX_BUF];
} skill_entry_struct;

/** Fire mode structure */
typedef struct _fire_mode {
    /** Item */
    int item;

    /** Ammunition */
    int amun;

    spell_entry_struct *spell;

    /** Skill */
    skill_entry_struct *skill;

    /** Name */
    char name[128];
} _fire_mode;

/**
 * A single help file entry.
 */
typedef struct hfile_struct {
    char *key;

    char *msg;

    size_t msg_len;

    uint8_t autocomplete;

    uint8_t autocomplete_wiz;

    UT_hash_handle hh;
} hfile_struct;

/**
 * Player's state.
 */
typedef enum player_state_t {
    /**
     * Just initialized the client.
     */
    ST_INIT,

    /**
     * Re-download metaserver list.
     */
    ST_META,

    /**
     * Close opened socket if any, prepare for connection.
     */
    ST_START,

    /**
     * Waiting to select a server to play on.
     */
    ST_WAITLOOP,

    /**
     * Selected a server, so start the connection procedure.
     */
    ST_STARTCONNECT,

    /**
     * Open a connection to the server.
     */
    ST_CONNECT,

    /**
     * Wait for crypto handshake to complete.
     */
    ST_WAITCRYPTO,

    /**
     * Start sending game data.
     */
    ST_START_DATA,

    /**
     * Wait for version information from the server.
     */
    ST_WAITVERSION,

    /**
     * Server version received.
     */
    ST_VERSION,

    /**
     * Wait for setup command from the server.
     */
    ST_WAITSETUP,

    /**
     * Request files listing.
     */
    ST_REQUEST_FILES_LISTING,

    /**
     * Wait for files listing request to complete.
     */
    ST_WAITREQUEST_FILES_LISTING,

    /**
     * Request files as necessary.
     */
    ST_REQUEST_FILES,

    /**
     * Choosing which account to login with.
     */
    ST_LOGIN,

    /**
     * Wait for login response.
     */
    ST_WAITLOGIN,

    /**
     * Choosing which character to play with.
     */
    ST_CHARACTERS,

    /**
     * Waiting for the relevant data packets to start playing.
     */
    ST_WAITFORPLAY,

    /**
     * Playing.
     */
    ST_PLAY
} player_state_t;

/* With this, we overrule bitmap loading params.
 * For example, we need for fonts an attached palette, and not the native vid
 * mode */

/** Use this when you want a colkey in a true color picture - color should be 0
 * */
#define SURFACE_FLAG_COLKEY_16M 2
#define SURFACE_FLAG_DISPLAYFORMAT 4
#define SURFACE_FLAG_DISPLAYFORMATALPHA 8

/* For custom cursors */
enum {
    MSCURSOR_MOVE = 1
};

#define IS_ENTER(_keysym) ((_keysym) == SDLK_RETURN || (_keysym) == SDLK_KP_ENTER)
#define IS_NEXT(_keysym) ((_keysym) == SDLK_TAB || IS_ENTER((_keysym)))

#endif
