#ifndef MY_COLLECT_H
#define MY_COLLECT_H

#include <stdbool.h>
#include "contiki.h"
#include "net/rime/rime.h"
#include "net/netstack.h"
#include "core/net/linkaddr.h"

#define BEACON_INTERVAL (CLOCK_SECOND*60)
#define BEACON_FORWARD_DELAY (random_rand() % CLOCK_SECOND)

#define RSSI_THRESHOLD -95
#define MAX_RETRANSMISSIONS 1

enum packet_type {
    upward_data_packet = 0,
    downward_data_packet = 1,
    topology_report = 2
};

/* Connection object */
struct my_collect_conn {
    struct broadcast_conn bc;
    struct unicast_conn uc;
    const struct my_collect_callbacks* callbacks;
    linkaddr_t parent;
    struct ctimer beacon_timer;
    uint16_t metric;
    uint16_t beacon_seqn;
    uint8_t is_sink;  // 1: is_sink, 0: not_sink
};
typedef struct my_collect_conn my_collect_conn;

struct my_collect_callbacks {
    void (* recv)(const linkaddr_t *originator, uint8_t hops);
    void (* sr_recv)(struct my_collect_conn *ptr, uint8_t hops);
};

/* Initialize a collect connection
 *  - conn -- a pointer to a connection object
 *  - channels -- starting channel C (the collect uses two: C and C+1)
 *  - is_sink -- initialize in either sink or router mode
 *  - callbacks -- a pointer to the callback structure */
void my_collect_open(struct my_collect_conn*, uint16_t, bool, const struct my_collect_callbacks*);

// -------- COMMUNICATION FUNCTIONS --------

int  my_collect_send(struct my_collect_conn *c);
void bc_recv(struct broadcast_conn *conn, const linkaddr_t *sender);
void uc_recv(struct unicast_conn *c, const linkaddr_t *from);
void send_beacon(struct my_collect_conn*);

void forward_upward_data(my_collect_conn *conn, const linkaddr_t *sender);

/*
 Source routing send function:
 Params:
    c: pointer to the collection connection structure
    dest: pointer to the destination address
 Returns non-zero if the packet could be sent, zero otherwise.
*/
int sr_send(struct my_collect_conn*, const linkaddr_t*);

void beacon_timer_cb(void* ptr);

// -------- MESSAGE STRUCTURES --------

// Beacon message structure
struct beacon_msg {
    uint16_t seqn;
    uint16_t metric;
} __attribute__((packed));
typedef struct beacon_msg beacon_msg;

struct upward_data_packet_header { // Header structure for data packets
    linkaddr_t source;
    uint8_t hops;
    uint8_t piggy_len;  // 0 in case there is no piggybacking
} __attribute__((packed));
typedef struct upward_data_packet_header upward_data_packet_header;

#endif //MY_COLLECT_H
