#include <stdio.h>
#include <enet/enet.h>
#include "stream.c"

ENetPeer *peer;

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
            int client_num = read_int(p);
            int protocol = read_int(p);
            if(protocol != 257)
                exit(-2);
            int session_id = read_int(p);
            int has_password = read_int(p);
            send_packet("\0Michael\0\0\0", 11);
            break;
        }
        case 0x02: { // SV_WELCOME
            read_int(p);
            break;
        }
        case 0x1c: { // SV_CLIENTPING
            read_int(p);
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

        default:
            printf("Fail (%d) %d: %02x\n", p->offset, p->data[0], token);
            break;
    }
}

int main()
{
    if(enet_initialize())
        return -1;

    ENetHost *client = enet_host_create(NULL, 2, 0, 0);
    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 28785;

    peer = enet_host_connect(client, &address, 2);
    ENetEvent event;
    if(enet_host_service(client, &event, 5000) <= 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
        enet_peer_reset(peer);
        return -1;
    }

    while(1) {
        if(enet_host_service(client, &event, 0) <= 0) continue;
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                if(event.channelID != 1) continue;
                packet *p = malloc(sizeof(packet));
                p->offset = 0;
                p->length = event.packet->dataLength;
                p->data = event.packet->data;
                parse_messages(p, -1);
                free(p);
                break;
            default:
                printf("Received event: %d.\n", event.type);
                break;
        }
        enet_packet_destroy(event.packet);
    }

    enet_host_destroy(client);
    atexit(enet_deinitialize);
    return 0;
}

