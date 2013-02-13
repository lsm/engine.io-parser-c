//
//  engine.io_parser.h
//  C implementation of Engine.IO protocol
//
//  Created by Liu Senmiao on 2/10/13.
//  Copyright (c) 2013 Liu Senmiao. All rights reserved.
//

#ifndef engine_io_parser_h
#define engine_io_parser_h
#ifdef __cplusplus
extern "C" {
#endif

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

/**
 * Packet types.
 */
enum {
    EIO_OPEN = 0,
    EIO_CLOSE = 1,
    EIO_PING = 2,
    EIO_PONG = 3,
    EIO_MESSAGE = 4,
    EIO_UPGRADE = 5,
    EIO_NOOP = 6
} eio_type;

typedef struct _eio_packet_t eio_packet_t;

struct _eio_packet_t {
    int type;
    const char *data;
};


char *eio_encode_packet(char *encoded, const eio_packet_t *packet);
eio_packet_t *eio_decode_packet(eio_packet_t *packet, const char *encoded);

char *eio_encode_payload(char *payload, const eio_packet_t *packets, int num_packets);
eio_packet_t *eio_decode_payload(eio_packet_t *packets, const char *payload);

#ifdef __cplusplus
}
#endif
#endif
