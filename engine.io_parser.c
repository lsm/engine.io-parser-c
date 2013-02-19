/*
 * engine.io_parser.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "engine.io_parser.h"

#define EIO_LENGTH_SIZE sizeof(char) * sizeof(size_t)

static const char *eio_packet_types = "0123456";

/* Encodes a single packet into string.
 *
 *  <packet type id>[<data>]
 *
 * Example:
 *  
 *  0
 *  2ping
 *  4Hello world
 */
char *
eio_encode_packet(char *encoded, const eio_packet_t *packet)
{
    uint8_t t = packet->type;
    char *p = encoded;
    char type;
    
    if (t >= EIO_OPEN && t <= EIO_NOOP) {
        type = eio_packet_types[t];
        *p++ = type;
        
        if (strlen(packet->data) > 0) {
            strcpy(p, packet->data);
        }
    }
    
    return encoded;
}

/* Decodes an encoded string to packet.
 *
 * Returns NULL pointer for invalid format.
 */
eio_packet_t *
eio_decode_packet(eio_packet_t *packet, const char *encoded)
{
    uint8_t type;
    const char *c = strchr(eio_packet_types, encoded[0]);
    
    if (c != NULL) {
        type = c - eio_packet_types;
    } else {
        packet = NULL;
        return packet;
    }
    
    packet->type = (uint8_t) type;
    
    if (strlen(encoded) > 1) {
        packet->data = encoded + 1;
    }
    
    return packet;
}

/* Encodes multiple packets (payload) into string.
 * This is for transport which doesn't have framing support.
 *
 *     <length>:data
 *
 * Example:
 *
 *     12:4hello world5:2ping
 */
char *
eio_encode_payload(char *encoded, const eio_packet_t *packets, uint32_t num_packets)
{
    if (num_packets == 0) {
        return strcpy(encoded, "0:");
    }
    
    uint32_t i;
    size_t len;
    char length[EIO_LENGTH_SIZE + 1];
    
    for (i = 0; i < num_packets; i++) {
        len = strlen(packets[i].data);
        char packet_str[len + 2];
        eio_encode_packet(packet_str, packets + i);
        sprintf(length, "%lu", strlen(packet_str));
        strcat(encoded, length);
        strcat(encoded, ":");
        strcat(encoded, packet_str);
    }
    
    return encoded;
}

/* Decodes data when a payload is maybe expected.
 *
 * Returns NULL pointer for invalid format.
 */
eio_packet_t *
eio_decode_payload(eio_packet_t *packets, const char *encoded)
{
    eio_packet_t *p = packets;
    size_t len = strlen(encoded);
    
    if (len == 0) {
        return NULL;
    }
    
    uint32_t n;
    char length[EIO_LENGTH_SIZE + 1];
    char nc[EIO_LENGTH_SIZE + 1];
    char c;
    
    for (size_t i = 0; i < len; i++) {
        c = encoded[i];
        
        if (':' != c) {
            sprintf(length, "%s%c", length, c);
        } else {
            n = (uint32_t) strtol(length, NULL, 10);
            sprintf(nc, "%d", n);
            
            if (0 == length[0] || strcmp(length, nc) != 0) {
                return NULL;
            }
            
            char *msg = malloc(n + 1);
            if (msg == NULL) {
                return NULL;
            }
            strncpy(msg, encoded + i + 1, n);
            msg[n] = '\0';
            
            if (n != strlen(msg)) {
                return NULL;
            }
            
            if (strlen(msg) > 0) {
                eio_packet_t *packet = eio_decode_packet(p++, msg);
                if (packet == NULL) {
                    return NULL;
                }
            }
            
            i += n;
            length[0] = '\0';
        }
    }
    
    if (length[0] != '\0') {
        return NULL;
    }
    
    return packets;
}
