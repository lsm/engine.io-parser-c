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


char *eio_encode_packet(char *encoded, const eio_packet_t *packet) {
    int t = packet->type;
    char *p = encoded;
    
    if (t >= EIO_OPEN && t <= EIO_NOOP) {
        // type char
        char type = eio_packet_types[t];
        *p++ = type;
        
        if (strlen(packet->data) > 0) {
            strcpy(p, packet->data);
        }
    }
    
    return encoded;
}

eio_packet_t *eio_decode_packet(eio_packet_t *packet, const char *encoded) {
    char type;
    const char *c = strchr(eio_packet_types, encoded[0]);
    
    if (c != NULL) {
        type = c - eio_packet_types;
    } else {
        packet = NULL;
        return packet;
    }
    
    packet->type = type;
    
    if (strlen(encoded) > 1) {
        packet->data = encoded + 1;
    }
    
    return packet;
}

char *eio_encode_payload(char *encoded, const eio_packet_t *packets, int num_packets) {
    if (num_packets == 0) {
        return strcpy(encoded, "0:");
    }
    
    char length[EIO_LENGTH_SIZE + 1];
    
    for (int i = 0; i < num_packets; i++) {
        size_t len = strlen(packets[i].data);
        // 1 character for type and 1 for null-terminater
        char packet_str[len + 2];
        eio_encode_packet(packet_str, packets + i);
        // <length>:data
        sprintf(length, "%lu", strlen(packet_str));
        strcat(encoded, length);
        strcat(encoded, ":");
        strcat(encoded, packet_str);
    }
    
    return encoded;
}

eio_packet_t *eio_decode_payload(eio_packet_t *packets, const char *encoded) {
    eio_packet_t *p = packets;
    size_t len = strlen(encoded);
    
    if (len == 0) {
        return NULL;
    }
    
    int n;
    char length[EIO_LENGTH_SIZE + 1];
    char nc[EIO_LENGTH_SIZE + 1];
    
    for (int i = 0; i < len; i++) {
        char c = encoded[i];
        
        if (':' != c) {
            sprintf(length, "%s%c", length, c);
        } else {
            n = (int) strtol(length, NULL, 10);
            sprintf(nc, "%d", n);
            
            if (0 == length[0] || strcmp(length, nc) != 0) {
                return NULL;
            }
            
            // slice packet data from string
            char *msg = malloc(n + 1);
            strncpy(msg, encoded + i + 1, n);
            msg[n] = '\0';
            
            if (n != strlen(msg)) {
                return NULL;
            }
            
            if (strlen(msg) > 0) {
                if (eio_decode_packet(p++, msg) == NULL) {
                    return NULL;
                }
            }
            
            i += n;
            length[0] = 0;
        }
    }
    
    if (length[0] != 0) {
        return NULL;
    }
    
    return packets;
}

