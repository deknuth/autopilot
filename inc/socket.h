#ifndef SOCKET_H
#define SOCKET_H

#define TCP_PORT    "1111"

extern void *TcpServer(void *arg);

typedef struct station{
    unsigned int index;
    double lat;
    double lon;
}SSTA,*PSSTA;

#endif // SOCKET_H
