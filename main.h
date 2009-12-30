#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <enet/enet.h>

#include "stream.h"
#include "demo.h"

#define BUFFER_SIZE 64

ENetPeer *peer;
ENetHost *client;
int bot_client_num;
char *map_name;
demo *d;
