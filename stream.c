typedef struct {
    uint8_t *data;
    int offset;
    int length;
} packet;

int read_char(packet *p)
{
    if(p->offset > p->length - 1)
        return -1;
    return p->data[p->offset ++];
}

int read_int(packet *p)
{
    int n = (char)read_char(p);
    if(n == -128)
        return read_char(p) | ((char)read_char(p) << 8);
    if(n == -127)
        return read_char(p) | (read_char(p) << 8) | (read_char(p) << 16) | (read_char(p) << 24);
    return n;
}

int read_uint(packet *p)
{
    int n = read_char(p);
    for(int i = 7; i < 28; i += 7)
        if(n & (1 << i))
            n += (read_char(p) << i) << (1 << i);
    if(n & (1 << 28))
        return n | 0xF0000000;
    return n;
}

void read_string(packet *p, char *in)
{
    do
        *in = read_char(p);
    while(*in ++);
}

void sub_buffer(packet *p, packet *p2, int length)
{
    p2->data = p->data + p->offset;
    p->offset += length;
    p2->length = length;
}
