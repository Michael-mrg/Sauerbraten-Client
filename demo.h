#include <time.h>
#include <string.h>
#include <zlib.h>
#include <stdio.h>
#include <enet/enet.h>

#include "stream.h"

typedef struct {
    char *map_name;
    char *file_name;
    const char *prefix;
    gzFile *file;
} demo;

demo *demo_new(const char *prefix);
void demo_record_timestamp(demo *d, clock_t time, int id, int len);
void demo_record_packet(demo *d, ENetPacket *packet);
void demo_set_map_name(demo *d, const char *map_name);
void demo_close(demo *d);
