#ifndef __STREAM_H__
#define __STREAM_H__
#include <inttypes.h>

typedef struct {
    uint8_t *data;
    int offset;
    int length;
} packet;

int read_char(packet *p);
int read_int(packet *p);
int read_uint(packet *p);
void read_string(packet *p, char *in);
void sub_buffer(packet *p, packet *p2, int length);
#endif
