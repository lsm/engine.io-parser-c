/*
 * engine.io_parser.h
 * C implementation of Engine.IO protocol
 *
 * Copyright (c) 2013 Liu Senmiao. All rights reserved.
 *
 * (The MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _ENGINE_IO_PARSER_H_
#define _ENGINE_IO_PARSER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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
    uint8_t type;
    const char *data;
};


char *
eio_encode_packet(char *encoded, const eio_packet_t *packet);
eio_packet_t *
eio_decode_packet(eio_packet_t *packet, const char *encoded);

char *
eio_encode_payload(char *payload, const eio_packet_t *packets, uint32_t num_packets);
eio_packet_t *
eio_decode_payload(eio_packet_t *packets, const char *payload);

#ifdef __cplusplus
}
#endif
#endif
