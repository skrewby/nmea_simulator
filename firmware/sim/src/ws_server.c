#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "civetweb.h"
#include "jansson.h"
#include "ws_server.h"

/* Global options for this example. */
static const char WS_URL[] = "/ws";
static const char *SERVER_OPTIONS[] = {
    "listening_ports", "8081", "num_threads", "10", "document_root", FIRMWARE_WEB_ROOT, NULL, NULL
};

#define WS_MAX_CLIENTS 8
static struct mg_connection *clients[WS_MAX_CLIENTS];
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Define websocket sub-protocols. */
/* This must be static data, available between mg_start and mg_stop. */
static const char subprotocol_bin[] = "Company.ProtoName.bin";
static const char subprotocol_json[] = "Company.ProtoName.json";
static const char *subprotocols[] = { subprotocol_bin, subprotocol_json, NULL };
static struct mg_websocket_subprotocols wsprot = { 2, subprotocols };

static struct mg_context *ctx;

/* User defined data structure for websocket client context. */
struct tClientContext {
    uint32_t connectionNumber;
    uint32_t demo_var;
};

/* Handler for new websocket connections. */
static int ws_connect_handler(const struct mg_connection *conn, void *user_data) {
    (void)user_data; /* unused */

    /* Allocate data for websocket client context, and initialize context. */
    struct tClientContext *wsCliCtx =
        (struct tClientContext *)calloc(1, sizeof(struct tClientContext));
    if (!wsCliCtx) {
        /* reject client */
        return 1;
    }
    static uint32_t connectionCounter = 0; /* Example data: client number */
    wsCliCtx->connectionNumber = __sync_add_and_fetch(&connectionCounter, 1);
    mg_set_user_connection_data(conn, wsCliCtx); /* client context assigned to connection */

    /* DEBUG: New client connected (but not ready to receive data yet). */
    const struct mg_request_info *ri = mg_get_request_info(conn);
    printf("Client %u connected with subprotocol: %s\n", wsCliCtx->connectionNumber,
           ri->acceptedWebSocketSubprotocol);

    return 0;
}

/* Handler indicating the client is ready to receive data. */
static void ws_ready_handler(struct mg_connection *conn, void *user_data) {
    (void)user_data; /* unused */

    /* Get websocket client context information. */
    struct tClientContext *wsCliCtx = (struct tClientContext *)mg_get_user_connection_data(conn);
    const struct mg_request_info *ri = mg_get_request_info(conn);
    (void)ri; /* in this example, we do not need the request_info */

    /* Track this connection so it receives future broadcasts. */
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            clients[i] = conn;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // Send the current board state to all connected clients to sync
    ws_server_publish_full_state(state_get());

    /* DEBUG: New client ready to receive data. */
    printf("Client %u ready to receive data\n", wsCliCtx->connectionNumber);
}

/* Handler indicating the client sent data to the server. */
static int ws_data_handler(struct mg_connection *conn, int opcode, char *data, size_t datasize,
                           void *user_data) {
    (void)user_data; /* unused */

    /* Get websocket client context information. */
    struct tClientContext *wsCliCtx = (struct tClientContext *)mg_get_user_connection_data(conn);
    const struct mg_request_info *ri = mg_get_request_info(conn);
    (void)ri; /* in this example, we do not need the request_info */

    /* DEBUG: Print data received from client. */
    const char *messageType = "";
    switch (opcode & 0xf) {
    case MG_WEBSOCKET_OPCODE_TEXT:
        messageType = "text";
        break;
    case MG_WEBSOCKET_OPCODE_BINARY:
        messageType = "binary";
        break;
    case MG_WEBSOCKET_OPCODE_PING:
        messageType = "ping";
        break;
    case MG_WEBSOCKET_OPCODE_PONG:
        messageType = "pong";
        break;
    }
    printf("Websocket received %lu bytes of %s data from client %u\n", (unsigned long)datasize,
           messageType, wsCliCtx->connectionNumber);

    return 1;
}

/* Handler indicating the connection to the client is closing. */
static void ws_close_handler(const struct mg_connection *conn, void *user_data) {
    (void)user_data; /* unused */

    /* Get websocket client context information. */
    struct tClientContext *wsCliCtx = (struct tClientContext *)mg_get_user_connection_data(conn);

    /* Stop tracking this connection. */
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < WS_MAX_CLIENTS; i++) {
        if (clients[i] == conn) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    /* DEBUG: Client has left. */
    printf("Client %u closing connection\n", wsCliCtx->connectionNumber);

    /* Free memory allocated for client context in ws_connect_handler() call. */
    free(wsCliCtx);
}

void ws_server_start() {
    /* Initialize CivetWeb library without OpenSSL/TLS support. */
    mg_init_library(0);

    /* Start the server using the advanced API. */
    struct mg_callbacks callbacks = { 0 };
    void *user_data = NULL;

    struct mg_init_data mg_start_init_data = { 0 };
    mg_start_init_data.callbacks = &callbacks;
    mg_start_init_data.user_data = user_data;
    mg_start_init_data.configuration_options = SERVER_OPTIONS;

    struct mg_error_data mg_start_error_data = { 0 };
    char errtxtbuf[256] = { 0 };
    mg_start_error_data.text = errtxtbuf;
    mg_start_error_data.text_buffer_size = sizeof(errtxtbuf);

    ctx = mg_start2(&mg_start_init_data, &mg_start_error_data);
    if (!ctx) {
        fprintf(stderr, "Cannot start server: %s\n", errtxtbuf);
        mg_exit_library();
        return;
    }

    /* Register the websocket callback functions. */
    mg_set_websocket_handler_with_subprotocols(ctx, WS_URL, &wsprot, ws_connect_handler,
                                               ws_ready_handler, ws_data_handler, ws_close_handler,
                                               user_data);

    printf("Websocket server running\n");
    printf("Serving HTML on http://localhost:8081\n");
}

void ws_server_stop(void) {
    if (ctx != NULL) {
        mg_stop(ctx);
        mg_exit_library();
        ctx = NULL;
    }
}

/** ==================================== Publish Functions ==================================== **/

static uint64_t now_us(void) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return (uint64_t)now.tv_sec * 1000000 + (uint64_t)now.tv_nsec / 1000;
}

static void publish(const char *kind, json_t *data) {
    json_t *envelope = json_pack("{s:i, s:I, s:s, s:o}", "version", 1, "ts_us",
                                 (json_int_t)now_us(), "kind", kind, "data", data);

    char *text = json_dumps(envelope, JSON_COMPACT);
    if (text != NULL) {
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < WS_MAX_CLIENTS; i++) {
            if (clients[i] != NULL) {
                mg_websocket_write(clients[i], MG_WEBSOCKET_OPCODE_TEXT, text, strlen(text));
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        free(text);
    }

    json_decref(envelope);
}

void ws_server_publish_gpio(const char *id, State_GPIO *gpio) {
    json_t *data = json_pack("{s:s, s:b}", "id", id, "state", gpio->high);
    publish("gpio", data);
}

void ws_server_publish_serial(const char *id, const char *buf, uint16_t len) {
    json_t *data = json_pack("{s:s, s:o}", "id", id, "data", json_stringn(buf, len));
    publish("serial", data);
}

void ws_server_publish_full_state(State *state) {
    json_t *gpio = json_object();
    for (int i = 0; i < BSP_LED_COUNT; i++) {
        json_object_set_new(gpio, state_led_name((BSP_LED)i), json_boolean(state->leds[i].high));
    }

    json_t *versions = json_object();
    json_object_set_new(versions, "firmware", json_string(FIRMWARE_VERSION));

    json_t *data = json_object();
    json_object_set_new(data, "versions", versions);
    json_object_set_new(data, "gpio", gpio);

    publish("full_state", data);
}
