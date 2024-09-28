/**
 * @file
 * Network graph widget header file.
 *
 * @author Alex Tokar
 */

#ifndef NETWORK_GRAPH_H
#define NETWORK_GRAPH_H

enum {
    NETWORK_GRAPH_TYPE_GAME, ///< Game data.
    NETWORK_GRAPH_TYPE_HTTP, ///< HTTP data.

    NETWORK_GRAPH_TYPE_MAX ///< Maximum number of data types.
};

enum {
    NETWORK_GRAPH_TRAFFIC_RX, ///< Incoming traffic.
    NETWORK_GRAPH_TRAFFIC_TX, ///< Outgoing traffic.

    NETWORK_GRAPH_TRAFFIC_MAX ///< Number of traffic types.
};

/* Prototypes */

void network_graph_update(int type, int traffic, size_t bytes);
void widget_network_graph_init(widgetdata *widget);

#endif
