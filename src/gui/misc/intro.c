/**
 * @file
 * The intro screen.
 *
 * @author Alex Tokar
 */

#include <global.h>
#include <toolkit/string.h>
#include <toolkit/curl.h>

/** How often to blink the eyes in ticks. */
#define EYES_BLINK_TIME (15 * 1000)
/** How long the eyes remain 'closed' (not drawn). */
#define EYES_BLINK_DELAY (200)

/**
 * Last server count to see when to re-create the servers list. Since the
 * metaserver code uses threading so the whole program doesn't lock up,
 * we need to do it like this.
 */
static size_t last_server_count = 0;

/** cURL request when downloading news from the site. */
static curl_request_t *news_request = NULL;

/** Last time the eyes blinked. */
static uint32_t eyes_blink_ticks = 0;
/** Whether to draw the eyes. */
static uint8_t eyes_draw = 1;
/** Button buffer. */
static button_struct button_play, button_refresh, button_server, button_settings, button_update, button_help, button_credits, button_quit;

/** The news list. */
static list_struct *list_news = NULL;
/** The servers list. */
static list_struct *list_servers = NULL;

/**
 * Handle enter key being pressed in the servers list.
 * @param list
 * The servers list.
 */
static void list_handle_enter(list_struct *list, SDL_Event *event)
{
    /* Servers list? */
    if (list == list_servers) {
        char number[16];
        uint32_t version, i;
        size_t pos;

        /* Get selected server. */
        selected_server = server_get_id(list->row_selected - 1);

        if (selected_server == NULL) {
            return;
        }

        for (pos = 0, i = 0, version = 0;
                string_get_word(selected_server->version, &pos, '.', number,
                sizeof(number), 0); i++) {
            version += atoi(number) << (i * CHAR_BIT);
        }

        if (version != 0 && version < SERVER_VERSION) {
            draw_info(COLOR_RED, "The server is outdated; "
                    "choose a different one.");
            return;
        }

        login_start();
    } else if (list == list_news) {
        if (list->text && list->text[list->row_selected - 1]) {
            game_news_open(list->text[list->row_selected - 1][0]);
        }
    }
}

/** @copydoc list_struct::esc_handle_func */
static void list_handle_esc(list_struct *list)
{
    (void) list;

    exit(0);
}

/** @copydoc list_struct::text_color_hook */
static void
list_text_color (struct list_struct *list,
                 uint32_t            row,
                 uint32_t            col,
                 const char        **color,
                 const char        **color_shadow)
{
    server_struct *server = server_get_id(row);
    SOFT_ASSERT(server != NULL, "Server on row %u is NULL", row);

    if (!server->is_meta) {
        return;
    }

    /* Default to gray color for the server entry.*/
    *color = COLOR_GRAY;

    /* If the server has a crypto port, check its level of trustworthiness. */
    if (server->port_crypto != -1) {
        /* Servers with a certificate have a higher level of trust. */
        if (server->cert_info != NULL) {
            /* However, check if the certificate has IP addresses; if it
             * doesn't, we can't be quite sure that we're connecting to the
             * right host (DNS spoofing is a possibility, albeit an unlikely
             * one). */
            if (server->cert_info->ipv4_address == NULL ||
                server->cert_info->ipv6_address == NULL) {
                *color = COLOR_YELLOW;
            } else {
                *color = COLOR_GREEN;
            }
        } else if (server->cert_pubkey != NULL) {
            *color = COLOR_ORANGE;
        }
    }
}

/**
 * Deinitialize data used by the intro screen.
 */
void intro_deinit(void)
{
    button_destroy(&button_play);
    button_destroy(&button_refresh);
    button_destroy(&button_server);
    button_destroy(&button_settings);
    button_destroy(&button_update);
    button_destroy(&button_help);
    button_destroy(&button_credits);
    button_destroy(&button_quit);

    list_remove(list_servers);
    list_servers = NULL;

    list_remove(list_news);
    list_news = NULL;
}

/**
 * Show the main GUI after starting the client -- servers list, chat box,
 * connecting to server, etc.
 */
void intro_show(void)
{
    SDL_Surface *texture;
    int x, y;
    size_t server_count;
    server_struct *node;
    char buf[MAX_BUF];
    SDL_Rect box;

    sound_start_bg_music("intro.ogg", setting_get_int(OPT_CAT_SOUND, OPT_VOLUME_MUSIC), -1);

    texture = TEXTURE_CLIENT("intro");

    /* Background */
    surface_show(ScreenSurface, 0, 0, NULL, texture);
    textwin_show(ScreenSurface, texture->w, 1, ScreenSurface->w - texture->w - 2, ScreenSurface->h - 3);

    /* Calculate whether to show the eyes or not. Blinks every
     * EYES_BLINK_TIME ticks, then waits EYES_BLINK_DELAY ticks until
     * showing the eyes again. */
    if (SDL_GetTicks() - eyes_blink_ticks >= (eyes_draw ? EYES_BLINK_TIME : EYES_BLINK_DELAY)) {
        eyes_blink_ticks = SDL_GetTicks();
        eyes_draw++;
    }

    if (eyes_draw) {
        SDL_Rect src_box;

        src_box.x = 0;
        src_box.y = eyes_draw - 1;
        src_box.w = TEXTURE_CLIENT("eyes")->w;
        src_box.h = TEXTURE_CLIENT("eyes")->h;
        surface_show(ScreenSurface, texture->w - 90, 310 + src_box.y, &src_box, TEXTURE_CLIENT("eyes"));

        if (eyes_draw > 1) {
            eyes_draw++;

            if (eyes_draw > src_box.h) {
                eyes_draw = 1;
            }
        }
    }

    texture = TEXTURE_CLIENT("servers_bg");
    x = 15;
    y = ScreenSurface->h - texture->h - 5;
    surface_show(ScreenSurface, x, y, NULL, texture);

    server_count = server_get_count();

    /* Create the buttons. */
    if (!list_servers) {
        button_create(&button_play);
        button_create(&button_refresh);
        button_create(&button_server);
        button_create(&button_settings);
        button_create(&button_update);
        button_create(&button_help);
        button_create(&button_credits);
        button_create(&button_quit);
    }

    /* List doesn't exist or the count changed? Create new list. */
    if (!list_servers || last_server_count != server_count) {
        size_t i;

        /* Remove it if it exists already. */
        if (list_servers) {
            list_remove(list_servers);
        }

        /* Create the servers list. */
        list_servers = list_create(11, 3, 8);
        list_servers->handle_enter_func = list_handle_enter;
        list_servers->handle_esc_func = list_handle_esc;
        list_servers->text_color_hook = list_text_color;
        list_scrollbar_enable(list_servers);
        list_set_column(list_servers, 0, 295, 7, "Server", -1);
        list_set_column(list_servers, 1, 50, 9, "Port", 1);
        list_set_column(list_servers, 2, 46, 7, "Players", 1);

        /* Add the servers to the list. */
        for (i = 0; i < server_count; i++) {
            node = server_get_id(i);

            list_add(list_servers, i, 0, node->name);
            snprintf(VS(buf),
                     "%d",
                     node->port_crypto == -1 ? node->port : node->port_crypto);
            list_add(list_servers, i, 1, buf);

            if (node->player >= 0) {
                snprintf(buf, sizeof(buf), "%d", node->player);
            } else {
                strcpy(buf, "-");
            }

            list_add(list_servers, i, 2, buf);
        }

        /* Store the new count. */
        last_server_count = server_count;
    }

    /* Actually draw the list. */
    list_show(list_servers, x + 12, y + 8);
    node = server_get_id(list_servers->row_selected - 1);

    /* Do we have any selected server? If so, show its version and
     * description. */
    if (node) {
        snprintf(buf, sizeof(buf), "Version: %s", node->version);
        text_show_shadow(ScreenSurface, FONT_ARIAL10, buf, x + 13, y + 185, COLOR_HGOLD, COLOR_BLACK, 0, NULL);

        box.w = 410;
        box.h = 48;
        text_show(ScreenSurface, FONT_ARIAL10, node->desc, x + 13, y + 197, COLOR_WHITE, TEXT_WORD_WRAP | TEXT_MARKUP, &box);
    }

    /* Show whether we are connecting to the metaserver or not. */
    if (ms_connecting(-1)) {
        text_show_shadow(ScreenSurface, FONT_ARIAL10, "Connecting to metaserver, please wait...", x + 105, y + 8, COLOR_HGOLD, COLOR_BLACK, 0, NULL);
    } else {
        text_show_shadow(ScreenSurface, FONT_ARIAL10, "Select a secure server.", x + 196, y + 8, COLOR_GREEN, COLOR_BLACK, 0, NULL);
    }

    texture = TEXTURE_CLIENT("servers_bg_over");
    surface_show(ScreenSurface, x, y, NULL, texture);

    x += texture->w + 20;
    texture = TEXTURE_CLIENT("news_bg");
    surface_show(ScreenSurface, x, y, NULL, texture);

    box.w = texture->w;
    box.h = 0;
    text_show_shadow(ScreenSurface, FONT_SERIF12, "Game News", x, y + 10, COLOR_HGOLD, COLOR_BLACK, TEXT_ALIGN_CENTER, &box);

    /* No list yet, make one and start downloading the data. */
    if (!list_news) {
        /* Start downloading. */
        news_request = curl_request_create(clioption_settings.game_news_url,
                                           CURL_PKEY_TRUST_ULTIMATE);
        curl_request_start_get(news_request);

        list_news = list_create(18, 1, 8);
        list_news->focus = 0;
        list_news->handle_enter_func = list_handle_enter;
        list_news->handle_esc_func = list_handle_esc;
        list_set_column(list_news, 0, 150, 7, NULL, -1);
        list_set_font(list_news, FONT_ARIAL10);
    }

    /* Download in progress? */
    if (news_request != NULL) {
        curl_state_t state = curl_request_get_state(news_request);
        /* Finished downloading, parse the data. */
        if (state == CURL_STATE_OK) {
            char *body = curl_request_get_body(news_request, NULL);
            if (body != NULL) {
                uint32_t i = 0;
                char *cp = strtok(body, "\n");
                while (cp != NULL) {
                    list_add(list_news, i++, 0, cp);
                    cp = strtok(NULL, "\n");
                }
            }
        }

        /* Finished downloading or there was an error: clean up in either
         * case. */
        if (state != CURL_STATE_INPROGRESS) {
            curl_request_free(news_request);
            news_request = NULL;
        }
    }

    /* Show the news list. */
    list_show(list_news, x + 13, y + 10);

    button_play.x = button_refresh.x = button_server.x = button_settings.x = button_update.x = button_help.x = button_credits.x = button_quit.x = 489;
    y += 2;

    button_play.y = y + 10;
    button_show(&button_play, "Play");

    button_refresh.y = y + 35;
    button_show(&button_refresh, "Refresh");

    button_server.y = y + 60;
    button_show(&button_server, "Server");

    button_settings.y = y + 86;
    button_show(&button_settings, "Settings");

    button_update.y = y + 110;
    button_show(&button_update, "Update");

    button_help.y = y + 135;
    button_show(&button_help, "Help");

    button_credits.y = y + 160;
    button_show(&button_credits, "Credits");

    button_quit.y = y + 224;
    button_show(&button_quit, "Quit");

    if (clioption_settings.connect[0] && cpl.state < ST_STARTCONNECT) {
        size_t i;

        for (i = 0; i < server_count; i++) {
            node = server_get_id(i);

            if (strcasecmp(clioption_settings.connect[0], node->name) == 0) {
                list_servers->row_selected = i + 1;

                if (!clioption_settings.reconnect) {
                    efree(clioption_settings.connect[0]);
                    clioption_settings.connect[0] = NULL;
                }

                event_push_key_once(SDLK_RETURN, 0);
                break;
            }
        }
    }
}

/**
 * Handle event in the main screen.
 * @param event
 * The event to handle.
 * @return
 * 1 if the event was handled, 0 otherwise.
 */
int intro_event(SDL_Event *event)
{
    if (!list_servers) {
        return 0;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON(1)) {
        if (LIST_MOUSE_OVER(list_news, event->motion.x, event->motion.y)) {
            list_news->focus = 1;
            list_servers->focus = 0;
        } else if (LIST_MOUSE_OVER(list_servers, event->motion.x, event->motion.y)) {
            list_servers->focus = 1;
            list_news->focus = 0;
        }
    }

    if (button_event(&button_play, event)) {
        list_handle_enter(list_servers, event);
        return 1;
    } else if (button_event(&button_refresh, event)) {
        if (!ms_connecting(-1)) {
            cpl.state = ST_META;
        }

        return 1;
    } else if (button_event(&button_server, event)) {
        server_add_open();
        return 1;
    } else if (button_event(&button_settings, event)) {
        settings_open();
        return 1;
    } else if (button_event(&button_update, event)) {
        updater_open();
        return 1;
    } else if (button_event(&button_help, event)) {
        help_show("main screen");
        return 1;
    } else if (button_event(&button_credits, event)) {
        credits_show();
        return 1;
    } else if (button_event(&button_quit, event)) {
        exit(0);
        return 1;
    } else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_TAB && list_news) {
        int news_focus = 0;

        if (list_servers->focus) {
            news_focus = 1;
        }

        list_news->focus = news_focus;
        list_servers->focus = !news_focus;
    } else if (list_handle_keyboard(list_news && list_news->focus ? list_news : list_servers, event)) {
        return 1;
    } else if (list_handle_mouse(list_news, event)) {
        return 1;
    } else if (list_handle_mouse(list_servers, event)) {
        return 1;
    }

    return 0;
}
