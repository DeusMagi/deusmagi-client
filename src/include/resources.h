/**
 * @file
 * Resource management header file.
 *
 * @author Alex Tokar
 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <openssl/sha.h>
#include <toolkit/curl.h>

typedef struct resource {
    UT_hash_handle hh;

    char *name;

    unsigned char md[SHA512_DIGEST_LENGTH];

    unsigned char digest[SHA512_DIGEST_LENGTH * 2 + 1];

    curl_request_t *request;

    bool loaded:1;
} resource_t;

/* Function prototypes */

void
resources_init (void);
void
resources_deinit(void);
void
resources_reload(void);
resource_t *
resources_find(const char *name);
resource_t *
resources_find_by_md(const unsigned char *md);
void
socket_command_resource(uint8_t *data, size_t len, size_t pos);
bool
resources_is_ready(resource_t *resource);

#endif
