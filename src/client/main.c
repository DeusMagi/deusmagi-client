/**
 * @file
 * Client main related functions.
 */

#include <global.h>
#include <region_map.h>
#include <toolkit/packet.h>
#include <toolkit/string.h>
#include <toolkit/clioptions.h>
#include <toolkit/path.h>
#include <resources.h>
#include <toolkit/signals.h>
#include <toolkit/colorspace.h>
#include <toolkit/binreloc.h>
#include <toolkit/datetime.h>
#include <toolkit/socket_crypto.h>
#include <toolkit/x11.h>
#include <cmake.h>

/** The main screen surface. */
SDL_Window *ScreenWindow;
SDL_Renderer *ScreenRenderer;
SDL_Surface *ScreenSurface;
SDL_Texture *ScreenTexture;
/** Server's attributes */
struct sockaddr_in insock;
/** Client socket. */
client_socket_t csocket;

/** Our selected server that we want to connect to. */
server_struct *selected_server = NULL;

/** System time counter in ms since program start. */
uint32_t LastTick;

texture_struct *cursor_texture;
int cursor_x = -1;
int cursor_y = -1;

/* update map area */
int map_redraw_flag;
int minimap_redraw_flag;

/** The stored "anim commands" we created out of anims.tmp. */
_anim_table *anim_table = NULL;
Animations *animations = NULL;
/** Number of animations. */
size_t animations_num = 0;

/** Size of the screen. */
struct screensize *Screensize;

/** Face data */
_face_struct FaceList[MAX_FACE_TILES];

/** The message animation structure. */
struct msg_anim_struct msg_anim;
/** Last time we sent keepalive command. */
static uint32_t last_keepalive;

/**
 * Command line option settings.
 */
clioption_settings_struct clioption_settings;

/**
 * Used to keep track of keepalive commands.
 */
typedef struct keepalive_data_struct {
    struct keepalive_data_struct *next; ///< Next keepalive data.

    uint32_t ticks; ///< When the keepalive command was sent.
    uint32_t id; ///< ID of the keepalive command.
} keepalive_data_struct;

static keepalive_data_struct *keepalive_data; ///< Keepalive data.
static int keepalive_id; ///< UID for sending keepalives.
static int keepalive_ping; ///< Last keepalive ping time.
static int keepalive_ping_avg; ///< Average keepalive ping time.
static int keepalive_ping_num; ///< Number of keepalive pings.

SDL_Cursor* system_cursor_arrow;
SDL_Cursor* system_cursor_hand;
SDL_Cursor* system_cursor_ibeam;
SDL_Cursor* system_cursor_sizeall;
SDL_Cursor* system_cursor_sizens;
SDL_Cursor* system_cursor_sizewe;

/**
 * Reset keepalive data.
 */
static void keepalive_reset(void)
{
    keepalive_data_struct *keepalive, *tmp;

    last_keepalive = SDL_GetTicks();
    keepalive_id = 0;
    keepalive_ping = 0;
    keepalive_ping_avg = 0;
    keepalive_ping_num = 0;

    LL_FOREACH_SAFE(keepalive_data, keepalive, tmp)
    {
        LL_DELETE(keepalive_data, keepalive);
        efree(keepalive);
    }
}

/**
 * Send a keepalive packet.
 */
static void keepalive_send(void)
{
    keepalive_data_struct *keepalive;
    packet_struct *packet;

    keepalive = emalloc(sizeof(*keepalive));
    keepalive->ticks = SDL_GetTicks();
    keepalive->id = ++keepalive_id;
    LL_PREPEND(keepalive_data, keepalive);

    packet = packet_new(SERVER_CMD_KEEPALIVE, 0, 0);
    packet_append_uint32(packet, keepalive->id);
    packet_enable_ndelay(packet);
    socket_send_packet(packet);
    last_keepalive = keepalive->ticks;
}

/**
 * Display ping statistics.
 */
void keepalive_ping_stats(void)
{
    draw_info(COLOR_WHITE, "\nPing statistics this session:");
    draw_info_format(COLOR_WHITE, "Keepalive TX: %d, RX: %d, missed: %d",
            keepalive_id, keepalive_ping_num,
            keepalive_id - keepalive_ping_num);
    draw_info_format(COLOR_WHITE, "Average ping: %d", keepalive_ping_avg);
    draw_info_format(COLOR_WHITE, "Last ping: %d", keepalive_ping);
}

/** @copydoc socket_command_struct::handle_func */
void socket_command_keepalive(uint8_t *data, size_t len, size_t pos)
{
    uint32_t id, ticks;
    keepalive_data_struct *keepalive, *tmp;

    id = packet_to_uint32(data, len, &pos);
    ticks = SDL_GetTicks();

    LL_FOREACH_SAFE(keepalive_data, keepalive, tmp)
    {
        if (id == keepalive->id) {
            LL_DELETE(keepalive_data, keepalive);

            keepalive_ping = ticks - keepalive->ticks;
            keepalive_ping_num++;
            keepalive_ping_avg = keepalive_ping_avg + ((keepalive_ping -
                    keepalive_ping_avg) / keepalive_ping_num);
            efree(keepalive);

            return;
        }
    }

    LOG(BUG, "Received unknown keepalive ID: %d", id);
}

/**
 * Initialize game data.
 */
static void init_game_data(void)
{
    init_map_data(0, 0, 0, 0);
    memset(FaceList, 0, sizeof(struct _face_struct) * MAX_FACE_TILES);

    init_keys();
    memset(&cpl, 0, sizeof(cpl));
    object_init();
    clear_player();

    memset(&MapData, 0, sizeof(MapData));

    msg_anim.message[0] = '\0';

    cpl.state = ST_INIT;
    map_redraw_flag = minimap_redraw_flag = 1;
    csocket.sc = NULL;

    metaserver_init();
    spells_init();
    skills_init();
}

/**
 * Game status chain.
 * @return
 * 1.
 */
static int game_status_chain(void)
{
    if (cpl.state == ST_INIT) {
        clear_map(true);
        cpl.state = ST_META;
    } else if (cpl.state == ST_META) {
        metaserver_clear_data();

        metaserver_add("localhost",
                       1728,
                       -1,
                       "Localhost",
                       "local",
                       "Localhost. Start server before you try to connect.");

        for (size_t i = 0; i < clioption_settings.servers_num; i++) {
            size_t pos = 0;
            char host[MAX_BUF];
            string_get_word(clioption_settings.servers[i],
                            &pos,
                            ' ',
                            VS(host),
                            0);
            char port[MAX_BUF];
            string_get_word(clioption_settings.servers[i],
                            &pos,
                            ' ',
                            VS(port),
                            0);
            char port_crypto[MAX_BUF];
            string_get_word(clioption_settings.servers[i],
                            &pos,
                            ' ',
                            VS(port_crypto),
                            0);
            int port_num = atoi(port);
            int port_crypto_num = atoi(port_crypto);
            metaserver_add(host,
                           port_num != 0 ? port_num : 1728,
                           port_crypto_num != 0 ? port_crypto_num : -1,
                           host,
                           "user server",
                           "Server from command line --server option.");
        }

        metaserver_get_servers();
        cpl.state = ST_START;
    } else if (cpl.state == ST_START) {
        if (csocket.sc != NULL) {
            client_socket_close(&csocket);
        }

        effect_stop();
        clear_map(true);
        resources_reload();
        map_redraw_flag = minimap_redraw_flag = 1;
        cpl.state = ST_WAITLOOP;
    } else if (cpl.state == ST_STARTCONNECT) {
        int port = selected_server->port;
        if (selected_server->port_crypto != -1) {
            port = selected_server->port_crypto;
        }
        draw_info_format(COLOR_GREEN,
                         "Trying server %s (%d)...",
                         selected_server->name,
                         port);
        keepalive_reset();
        cpl.state = ST_CONNECT;
    } else if (cpl.state == ST_CONNECT) {
        bool secure = false;
        int port = selected_server->port;
        if (selected_server->port_crypto != -1) {
            secure = true;
            port = selected_server->port_crypto;
        }

        /* Ensure we have a public key record. */
        if (secure && METASERVER_GET_PUBKEY(selected_server) == NULL) {
            draw_info_format(COLOR_RED,
                             "The server %s (%d) does not have a public key "
                             "record, refusing to connect.",
                             selected_server->name,
                             selected_server->port);
            cpl.state = ST_START;
            return 1;
        }

        if (!client_socket_open(&csocket,
                                selected_server->hostname,
                                port,
                                secure)) {
            draw_info(COLOR_RED, "Connection failed!");
            cpl.state = ST_START;
            return 1;
        }

        if (!metaserver_cert_verify_host(selected_server,
                                         socket_get_addr(csocket.sc))) {
            draw_info(COLOR_RED, "Failed to verify the IP address of the "
                                 "specified server - it is very likely the "
                                 "server has been compromised!");
            cpl.state = ST_START;
            return 1;
        }

        socket_thread_start();
        clear_player();

        if (secure) {
            packet_struct *packet = packet_new(SERVER_CMD_CRYPTO, 16, 0);
            packet_append_uint8(packet, CMD_CRYPTO_HELLO);
            socket_send_packet(packet);
            cpl.state = ST_WAITCRYPTO;
        } else {
            cpl.state = ST_START_DATA;
        }
    } else if (cpl.state == ST_START_DATA) {
        packet_struct *packet = packet_new(SERVER_CMD_VERSION, 16, 0);
        packet_append_uint32(packet, SOCKET_VERSION);
        socket_send_packet(packet);

        keepalive_send();
        cpl.state = ST_WAITVERSION;
    } else if (cpl.state == ST_VERSION) {
        packet_struct *packet;

        packet = packet_new(SERVER_CMD_SETUP, 256, 256);
        packet_append_uint8(packet, CMD_SETUP_SOUND);
        packet_append_uint8(packet, 1);
        packet_append_uint8(packet, CMD_SETUP_MAPSIZE);
        packet_append_uint8(packet, setting_get_int(OPT_CAT_MAP, OPT_MAP_WIDTH));
        packet_append_uint8(packet, setting_get_int(OPT_CAT_MAP, OPT_MAP_HEIGHT));
        packet_append_uint8(packet, CMD_SETUP_DATA_URL);
        packet_append_string_terminated(packet, "");
        socket_send_packet(packet);

        cpl.state = ST_WAITSETUP;
    } else if (cpl.state == ST_REQUEST_FILES_LISTING) {
        /* Retrieve the server files listing. */
        server_files_listing_retrieve();
        /* Load up the existing server files. */
        server_files_load(0);
        cpl.state = ST_WAITREQUEST_FILES_LISTING;
    } else if (cpl.state == ST_WAITREQUEST_FILES_LISTING) {
        if (server_files_listing_processed()) {
            cpl.state = ST_REQUEST_FILES;
        }
    } else if (cpl.state == ST_REQUEST_FILES) {
        if (server_files_processed()) {
            server_files_load(1);
            cpl.state = ST_LOGIN;
        }
    } else if (cpl.state == ST_WAITFORPLAY) {
        clear_map(true);
    }

    return 1;
}

/**
 * Play various action sounds.
 */
static void play_action_sounds(void)
{
    if (cpl.warn_statdown) {
        sound_play_effect("warning_statdown.ogg", 100);
        cpl.warn_statdown = 0;
    }

    if (cpl.warn_statup) {
        sound_play_effect("warning_statup.ogg", 100);
        cpl.warn_statup = 0;
    }

    if (cpl.warn_hp) {
        if (cpl.warn_hp == 2) {
            sound_play_effect("warning_hp2.ogg", 100);
        } else {
            sound_play_effect("warning_hp.ogg", 100);
        }

        cpl.warn_hp = 0;
    }
}

/**
 * Hook for detecting background music changes.
 */
static void sound_background_hook(void)
{
    WIDGET_REDRAW_ALL(MPLAYER_ID);
}

void clioption_settings_deinit(void)
{
    size_t i;

    for (i = 0; i < clioption_settings.servers_num; i++) {
        efree(clioption_settings.servers[i]);
    }

    if (clioption_settings.servers) {
        efree(clioption_settings.servers);
    }

    for (i = 0; i < clioption_settings.metaservers_num; i++) {
        efree(clioption_settings.metaservers[i]);
    }

    if (clioption_settings.metaservers) {
        efree(clioption_settings.metaservers);
    }

    for (i = 0; i < arraysize(clioption_settings.connect); i++) {
        if (clioption_settings.connect[i]) {
            efree(clioption_settings.connect[i]);
        }
    }

    if (clioption_settings.game_news_url) {
        efree(clioption_settings.game_news_url);
    }
}

/**
 * Description of the --server command.
 */
static const char *const clioptions_option_server_desc =
"Adds a server to the list of servers.\n\n"
"Usage:\n"
" --server=example.com";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_server (const char *arg,
                          char      **errmsg)
{
    clioption_settings.servers =
        erealloc(clioption_settings.servers,
                 sizeof(*clioption_settings.servers) *
                     (clioption_settings.servers_num + 1));
    clioption_settings.servers[clioption_settings.servers_num] =
        estrdup(arg);
    clioption_settings.servers_num++;
    return true;
}

/**
 * Description of the --metaserver command.
 */
static const char *const clioptions_option_metaserver_desc =
"Adds a metaserver to the list of metaserver that will be tried.\n\n"
"Usage:\n"
" --metaserver=example.com";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_metaserver (const char *arg,
                              char      **errmsg)
{
    clioption_settings.metaservers =
        erealloc(clioption_settings.metaservers,
                 sizeof(*clioption_settings.metaservers) *
                     (clioption_settings.metaservers_num + 1));
    clioption_settings.metaservers[clioption_settings.metaservers_num] =
        estrdup(arg);
    clioption_settings.metaservers_num++;
    return true;
}

/**
 * Description of the --connect command.
 */
static const char *const clioptions_option_connect_desc =
"Automatically connects to a server.\n\n"
"Usage:\n"
" --connect=<server>:<account>:<password>:<character>\n\n"
"Everything past the server is optional; you could for example only specify "
"the account, or specify everything but leave the password field empty to "
"still get a prompt for the password but still select the character "
"automatically.";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_connect (const char *arg,
                           char      **errmsg)
{
    char *cp = estrdup(arg);
    char *cps[4];
    size_t num = string_split(cp, cps, arraysize(cps), ':');

    for (size_t i = 0; i < num; i++) {
        if (*cps[i] == '\0') {
            continue;
        }

        clioption_settings.connect[i] = estrdup(cps[i]);
    }

    efree(cp);
    return true;
}

/**
 * Description of the --nometa command.
 */
static const char *clioptions_option_nometa_desc =
"Do not query the metaserver.";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_nometa (const char *arg,
                          char      **errmsg)
{
    metaserver_disable();
    return true;
}

/**
 * Description of the --text_debug command.
 */
static const char *clioptions_option_text_debug_desc =
"Enable text API debugging (shows bounding boxes for text that uses them).";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_text_debug (const char *arg,
                              char      **errmsg)
{
    text_enable_debug();
    return true;
}

/**
 * Description of the --widget_render_debug command.
 */
static const char *clioptions_option_widget_render_debug_desc =
"Enable widget rendering debugging (highlights widgets whenever they get "
"re-rendered).";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_widget_render_debug (const char *arg,
                                       char      **errmsg)
{
    widget_render_enable_debug();
    return true;
}

/**
 * Description of the --game_news_url command.
 */
static const char *clioptions_option_game_news_url_desc =
"Sets the game news URL. Typically this doesn't need to be changed.";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_game_news_url (const char *arg,
                                 char      **errmsg)
{
    clioption_settings.game_news_url = estrdup(arg);
    return true;
}

/**
 * Description of the --reconnect command.
 */
static const char *clioptions_option_reconnect_desc =
"On disconnect, will automatically attempt to reconnect to the "
"server/account/etc as specified with --connect.";
/** @copydoc clioptions_handler_func */
static bool
clioptions_option_reconnect (const char *arg,
                             char      **errmsg)
{
    clioption_settings.reconnect = 1;
    return true;
}

/**
 * The main function.
 * @param argc
 * Number of arguments.
 * @param argv[]
 * Arguments.
 * @return
 * 0
 */
int main(int argc, char *argv[])
{
    char *path;
    int done = 0, update, frames;
    uint32_t anim_tick, frame_start_time, elapsed_time, fps_limit,
            last_frame_ticks, last_memory_check;
    int fps_limits[] = {30, 60, 120, 0};

    toolkit_import(signals);
    signals_set_traceback_prefix(EXECUTABLE);

    toolkit_import(binreloc);
    toolkit_import(clioptions);
    toolkit_import(colorspace);
    toolkit_import(curl);
    toolkit_import(datetime);
    toolkit_import(logger);
    toolkit_import(math);
    toolkit_import(memory);
    toolkit_import(packet);
    toolkit_import(sha1);
    toolkit_import(socket);
    toolkit_import(socket_crypto);
    toolkit_import(string);
    toolkit_import(stringbuffer);
    toolkit_import(x11);
    
    path_fopen = client_fopen_wrapper;

    char version[MAX_BUF];
    package_get_version_full(VS(version));
    
    LOG(INFO, "Starting %s v%s ...", PACKAGE_NAME, PACKAGE_VERSION);
    
    /* Store user agent for cURL, including if this is a GNU/Linux build of
     * the client or a Windows one. */
    char user_agent[MAX_BUF];
#if defined(WIN32)
    snprintf(VS(user_agent), "Deus Magi Client (Win32)/%s (%d)",
             version, SOCKET_VERSION);
#elif defined(__GNUC__)
    snprintf(VS(user_agent), "Deus Magi Client (GNU/Linux)/%s (%d)",
             version, SOCKET_VERSION);
#else
    snprintf(VS(user_agent), "Deus Magi Client (Unknown)/%s (%d)",
             version, SOCKET_VERSION);
#endif

    curl_set_user_agent(user_agent);

    clioption_t *cli;

    /* Non-argument options */
    CLIOPTIONS_CREATE_ARGUMENT(cli, server, "Add a server to the list");
    CLIOPTIONS_CREATE_ARGUMENT(cli, metaserver, "Add a metaserver to the list");
    CLIOPTIONS_CREATE_ARGUMENT(cli, connect, "Connect to the specified server");
    CLIOPTIONS_CREATE_ARGUMENT(cli, game_news_url, "Set game news URL");

    /* Argument options*/
    CLIOPTIONS_CREATE(cli, nometa, "Disable querying the metaserver");
    CLIOPTIONS_CREATE(cli, text_debug, "Enable text API debugging");
    CLIOPTIONS_CREATE(cli, widget_render_debug,  "Enable widget debugging");
    CLIOPTIONS_CREATE(cli, reconnect,  "Reconnect automatically");

    memset(&clioption_settings, 0, sizeof(clioption_settings));

    path = file_path("deusmagi.cfg", "r");
    clioptions_load(path, NULL);
    efree(path);
    path = file_path("deusmagi-custom.cfg", "r");
    clioptions_load(path, NULL);
    efree(path);

    clioptions_parse(argc, argv);

    logger_open_log(LOG_FILE);

    upgrader_init();
    settings_init();
    init_game_data();

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        LOG(ERROR, "Couldn't initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    /* Start the system after starting SDL */
    video_init();
    system_start();
    sprite_init_system();
    text_init();
    texture_init();
    sound_init();
    cmd_aliases_init();
    keybind_load();
    load_mapdef_dat();
    image_init();
    server_files_init();
    toolkit_widget_init();
    resources_init();
    
    system_cursor_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    system_cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    system_cursor_ibeam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    system_cursor_sizeall = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    system_cursor_sizens = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    system_cursor_sizewe = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    
    StringBuffer *sb = stringbuffer_new();
    stringbuffer_append_printf(sb,
                               "%s/.deus magi/%s",
                               get_config_dir(),
                               version);
    path = stringbuffer_finish(sb);
    socket_crypto_set_path(path);
    efree(path);

    char buf[HUGE_BUF];
    snprintf(VS(buf), "Welcome to %s v%s", PACKAGE_NAME, PACKAGE_VERSION);
    
    draw_info(COLOR_HGOLD, buf);

    if (!x11_clipboard_register_events()) {
        draw_info(COLOR_RED, "Failed to initialize clipboard support, clipboard actions will not be possible.");
    }

    settings_apply();
    scrollbar_init();
    button_init();

    atexit(system_end);

    SDL_SetCursor(system_cursor_arrow);
    cursor_texture = texture_get(TEXTURE_TYPE_CLIENT, "cursor_default");

    sound_background_hook_register(sound_background_hook);

    LastTick = anim_tick = last_frame_ticks = last_memory_check =
            SDL_GetTicks();
    frames = 0;

    while (!done) {
        frame_start_time = SDL_GetTicks();
        done = Event_PollInputDevice();

        /* Have we been shutdown? */
        if (handle_socket_shutdown()) {
            if (cpl.state != ST_STARTCONNECT) {
                cpl.state = ST_START;
                /* Make sure no popup is visible. */
                popup_destroy_all();
            }

            continue;
        }

        /* Check the memory every 10 seconds. */
        if (SDL_GetTicks() - last_memory_check > 10 * 1000) {
            memory_check_all();
            last_memory_check = SDL_GetTicks();
        }

        if (cpl.state > ST_CONNECT) {
            /* Send keepalive command every 2 minutes. */
            if (SDL_GetTicks() - last_keepalive > (2 * 60) * 1000) {
                keepalive_send();
            }

            DoClient();
        }

        /* If not connected, walk through connection chain and/or wait for
         * action */
        if (cpl.state != ST_PLAY) {
            if (!game_status_chain()) {
                LOG(ERROR, "Error connecting: cpl.state: %d SocketError: %s "
                        "(%d)", cpl.state, strerror(s_errno), s_errno);
            }
        } else if (SDL_GetWindowFlags(ScreenWindow) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS)) {
            if (LastTick - anim_tick > 125) {
                anim_tick = LastTick;
                animate_objects();
                map_animate();
            }

            play_action_sounds();
        }

        update = 1; // 0

        /* Disabling this for now ...
         * - it appears to introduce glitches with mouse events
         *   and redrawing when the screen resolution is changed
        
        if (!(SDL_GetWindowFlags(ScreenWindow) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS))) {
        } else if (cpl.state == ST_PLAY) {
            static int old_cursor_x = -1, old_cursor_y = -1;

            if (widgets_need_redraw()) {
                update = 1;
            } else if (cursor_x != old_cursor_x || cursor_y != old_cursor_y) {
                update = 1;
                old_cursor_x = cursor_x;
                old_cursor_y = cursor_y;
            } else if (event_dragging_need_redraw()) {
                update = 1;
            } else if (popup_need_redraw()) {
                update = 1;
            } else if (tooltip_need_redraw()) {
                update = 1;
            } else if (map_redraw_flag || minimap_redraw_flag) {
                update = 1;
            } else if (map_anims_need_redraw()) {
                update = 1;
            }
        } else {
            update = 1;
        }
        
        */
        
        if (update) {
            SDL_FillRect(ScreenSurface, NULL, 0);
            SDL_SetRenderDrawColor(ScreenRenderer, 0, 0, 0, 255);
        }

        if (cpl.state <= ST_WAITFORPLAY) {
            intro_show();
        } else if (cpl.state == ST_PLAY) {
            process_widgets(update);
        }

        popup_render_all();
        tooltip_show();

        /* Show the currently dragged item. */
        if (event_dragging_check()) {
            int mx, my;
            float lx, ly;
            
            SDL_GetMouseState(&mx, &my);
            
            // map the mouse coordinates to our logical render size
            SDL_RenderWindowToLogical(ScreenRenderer, mx, my, &lx, &ly);
            mx = (int) lx;
            my = (int) ly;
            
            object_show_centered(ScreenSurface, object_find(cpl.dragging_tag),
                    mx, my, INVENTORY_ICON_SIZE, INVENTORY_ICON_SIZE, false);
        }

        /* Disabling this for now ...
         * - SDL is unable to hide the system cursor on WSL / x11
         * - the surface coordinates get messed up when changing resolution
         * - the rendering is slow and so the cursor is slightly out of sync
         
        if (!setting_get_int(OPT_CAT_CLIENT, OPT_SYSTEM_CURSOR) &&
            SDL_GetWindowFlags(ScreenWindow) & SDL_WINDOW_MOUSE_FOCUS) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            
            surface_show(ScreenSurface,
                         mx - texture_surface(cursor_texture)->w / 2,
                         my - texture_surface(cursor_texture)->h / 2,
                         NULL,
                         texture_surface(cursor_texture));
        }
        
        */

        texture_gc();
        font_gc();
        sprite_cache_gc();

        if (update) {
            SDL_UpdateTexture(ScreenTexture, NULL, ScreenSurface->pixels, ScreenSurface->pitch);
            SDL_RenderClear(ScreenRenderer);
            SDL_RenderCopy(ScreenRenderer, ScreenTexture, NULL, NULL);
            SDL_RenderPresent(ScreenRenderer);
        }

        LastTick = SDL_GetTicks();

        if (SDL_GetWindowFlags(ScreenWindow) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS)) {
            frames++;

            if (LastTick - last_frame_ticks >= 1000) {
                last_frame_ticks = LastTick;
                effect_frames(frames);
                frames = 0;
            }
        }

        elapsed_time = SDL_GetTicks() - frame_start_time;
        fps_limit = fps_limits[setting_get_int(OPT_CAT_CLIENT, OPT_FPS_LIMIT)];

        if (fps_limit != 0) {
            while (1) {
                if (elapsed_time < 1000 / fps_limit) {
                    SDL_Delay(MAX(1, 1000 / fps_limit - elapsed_time));

                    if (!(SDL_GetWindowFlags(ScreenWindow) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS)) && SDL_GetTicks() - frame_start_time < 1000) {
                        SDL_PumpEvents();
                        continue;
                    }
                }

                break;
            }
        }
    }

    return 0;
}
