#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <enet/enet.h>
#include "stream.h"

ENetPeer *peer;
ENetHost *client;
int bot_client_num;

void send_packet(char *str, int len)
{
    ENetPacket *packet = enet_packet_create(str, len, 0);
    enet_peer_send(peer, 1, packet);
}

void parse_messages(packet *p, int cn)
{
    int token = read_int(p);
    switch(token) {
        case 0x01: { // SV_SERVINFO
            bot_client_num = read_int(p);
            int protocol = read_int(p);
            if(protocol != 257)
                exit(-2);
            int session_id = read_int(p);
            int has_password = read_int(p);

            char *name = "[mVa]bot";
            char data[32];
            memset(data, '\0', 32);
            strcat(data+1, name);
            send_packet(data, 4 + strlen(name));
            break;
        }
        case 0x02: { // SV_WELCOME
            read_int(p);

            char data[] = "\x37\0\1";
            data[1] = bot_client_num;
            send_packet(data, 4);
            break;
        }
        case 0x11: { // SV_SPAWN
            for(int i = 0; i < 12; i ++)
                read_int(p);
            break;
        }
        case 0x1c: { // SV_CLIENTPING
            read_int(p);
            break;
        }
        case 0x1d: { // SV_TIMEUP
            int mins_left = read_int(p);
            printf("Minutes left: %d\n", mins_left);
            break;
        }
        case 0x22: { // SV_RESUME
            while(1) {
                int client_num = read_int(p);
                if(client_num < 0) break;
                for(int i = 0; i < 15; i ++)
                    read_int(p);
            }
            break;
        }
        case 0x51: { // SV_CLIENT
            int client_num = read_int(p);
            int length = read_uint(p);
            packet *p2 = malloc(sizeof(packet));
            sub_buffer(p, p2, length);
            parse_messages(p2, client_num);
            free(p2);
            break;
        }

        case 0x07: { // SV_CDIS
            read_int(p);
            break;
        }
        case 0x36: { // SV_CURRENTMASTER
            for(int i = 0; i < 2; i ++)
                read_int(p);
            break;
        }

        case 0x06: { // SV_SOUND
            read_int(p);
            break;
        }
        case 0x0b: { // SV_DIED
            for(int i = 0; i < 3; i ++)
                read_int(p);
            break;
        }
        case 0x0c: { // SV_DAMAGE
            for(int i = 0; i < 5; i ++)
                read_int(p);
            break;
        }
        case 0x0d: { // SV_HITPUSH
            for(int i = 0; i < 6; i ++)
                read_int(p);
            break;
        }
        case 0x0e: { // SV_SHOTFX
            for(int i = 0; i < 8; i ++)
                read_int(p);
            break;
        }

        case 0x3d: { // SV_REPAMMO
            for(int i = 0; i < 2; i ++)
                read_int(p);
            break;
        }
        
        case 0x20: { // SV_SERVMSG
            char str[256];
            read_string(p, str);
            printf("SERV: %s\n", str);
            break;
        }

        case 0x59: { // SV_INITAI
            for(int i = 0; i < 5; i ++)
                read_int(p);
            read_string(p, NULL);
            read_string(p, NULL);
            break;
        }

        default:
            for(int i = 0; i < 8; i ++)
                printf("%02x ", p->data[i]);
            printf("\nFail (%d): %02x\n", p->offset, token);
            if(0 && token == -1)
                exit(-2);
            break;
    }
}

void disconnect()
{
    char data[] = "\x07\0";
    data[1] = bot_client_num;
    send_packet(data, 3);

    enet_peer_disconnect(peer, 1);
    enet_host_flush(client);
    enet_peer_reset(peer);
    enet_host_destroy(client);
}

void disconnect_signal(int signum)
{
    exit(0);
}

int main()
{
    if(enet_initialize())
        return -1;

    client = enet_host_create(NULL, 2, 0, 0);
    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 28785;

    peer = enet_host_connect(client, &address, 2);
    ENetEvent event;
    if(enet_host_service(client, &event, 5000) <= 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
        enet_peer_reset(peer);
        return -1;
    }

    signal(SIGINT, disconnect_signal);
    atexit(disconnect);
    atexit(enet_deinitialize);

    while(1) {
        if(enet_host_service(client, &event, 0) <= 0) continue;
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                if(event.channelID == 1) {
                    packet *p = malloc(sizeof(packet));
                    p->offset = 0;
                    p->length = event.packet->dataLength;
                    p->data = event.packet->data;
                    parse_messages(p, -1);
                    free(p);
                }
        
                //int stamp[3] = { 0, event.channelID, event.packet->dataLength };
                break;
            default:
                printf("Received event: %d.\n", event.type);
                break;
        }
        enet_packet_destroy(event.packet);
    }

    return 0;
}

