/**
 * @file
 * Handles movement events.
 */

#include <global.h>
#include <toolkit/packet.h>

/**
 * Number of the possible directions.
 */
#define DIRECTIONS_NUM 9

/**
 * Directions to fire into.
 */
static const int directions_fire[DIRECTIONS_NUM] = {
    6, 5, 4, 7, 0, 3, 8, 1, 2
};

void client_send_fire(int num, tag_t tag)
{
    packet_struct *packet;

    packet = packet_new(SERVER_CMD_FIRE, 64, 64);
    packet_append_uint8(packet, directions_fire[num - 1]);

    if (tag) {
        packet_append_uint32(packet, tag);
    }

    socket_send_packet(packet);
}

void move_keys(int num)
{
    if (cpl.fire_on) {
        client_send_fire(num, 0);
    } else {
        packet_struct *packet;

        if (num == 5) {
            packet = packet_new(SERVER_CMD_CLEAR, 0, 0);
            socket_send_packet(packet);
        } else {
            packet = packet_new(SERVER_CMD_MOVE, 8, 0);
            packet_append_uint8(packet, num ? directions_fire[num - 1] : 0);
            packet_append_uint8(packet, cpl.run_on);
            socket_send_packet(packet);
        }
    }
}

/**
 * Transform tile coordinates into direction, which can be used as a
 * result for functions like move_keys() or ::directions_move (return
 * value - 1).
 * @param tx
 * Tile X.
 * @param ty
 * Tile Y.
 * @return
 * The direction, 1-9.
 */
int dir_from_tile_coords(int tx, int ty)
{
    int player_tile_x = setting_get_int(OPT_CAT_MAP, OPT_MAP_WIDTH) / 2, player_tile_y = setting_get_int(OPT_CAT_MAP, OPT_MAP_HEIGHT) / 2;
    int q, x, y;

    if (tx == player_tile_x && ty == player_tile_y) {
        return 5;
    }

    x = -(tx - player_tile_x);
    y = -(ty - player_tile_y);

    if (!y) {
        q = -300 * x;
    } else {
        q = x * 100 / y;
    }

    if (y > 0) {
        /* East */
        if (q < -242) {
            return 6;
        }

        /* Northeast */
        if (q < -41) {
            return 9;
        }

        /* North */
        if (q < 41) {
            return 8;
        }

        /* Northwest */
        if (q < 242) {
            return 7;
        }

        /* West */
        return 4;
    }

    /* West */
    if (q < -242) {
        return 4;
    }

    /* Southwest */
    if (q < -41) {
        return 1;
    }

    /* South */
    if (q < 41) {
        return 2;
    }

    /* Southeast */
    if (q < 242) {
        return 3;
    }

    /* East */
    return 6;
}
