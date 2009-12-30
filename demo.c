#include "demo.h"

demo *demo_new(const char *prefix)
{
    demo *d = malloc(sizeof(demo));
    d->prefix = prefix;
    d->file_name = "temp";
    d->map_name = NULL;
    d->file = gzopen(d->file_name, "w9");

    gzwrite(d->file, "SAUERBRATEN_DEMO", 16);
    int version[] = { 1, 257 };
    gzwrite(d->file, version, sizeof(version));
    return d;
}

void demo_record_timestamp(demo *d, clock_t time, int id, int len)
{
    int stamp[] = { (clock() - time) / 1000.0, id, len };
    gzwrite(d->file, stamp, sizeof(stamp));
}

void demo_record_packet(demo *d, ENetPacket *packet)
{
    gzwrite(d->file, packet->data, packet->dataLength);
}

void demo_set_map_name(demo *d, const char *map_name)
{
    if(d->map_name)
        free(d->map_name);
    d->map_name = malloc(strlen(map_name) + 1);
    strcpy(d->map_name, map_name);
}

void demo_close(demo *d)
{
    gzclose(d->file);
    char buffer[256];
    sprintf(buffer, "%s%d_%s.dmo", d->prefix, (int)time(NULL), d->map_name);
    printf("Writing \"%s\"\n", buffer);
    rename(d->file_name, buffer);
    if(d->map_name)
        free(d->map_name);
    free(d);
}

/*
void make_file_name() {
    memset(file_name, '\0', BUFFER_SIZE);
    sprintf(file_name, "%s%d_%s.dmo", file_prefix, file_num, map_name);
}

void close_file() {
    gzclose(f);
    char old_file[256];
    strcpy(old_file, file_name);
    make_file_name();
    if(strcmp(old_file, file_name))
        rename(old_file, file_name);
}

void next_file()
{
    if(f)
        close_file();
    else
        make_file_name();

    f = gzopen(file_name, "w9");
    gzwrite(f, "SAUERBRATEN_DEMO", 16);
    int version[] = { 1, 257 };
    gzwrite(f, version, sizeof(int)*2);
}
*/
