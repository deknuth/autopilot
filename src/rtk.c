#include "../inc/core.h"
int RtkSet(char *path)
{
    int fd;
    int res = 0;
    char buffer[128] = {"$GPGGA,010753.000,2813.9798,N,11255.8564,E,1,6,2.31,166.6,M,-19.4,M,,*76"};
    char end[2] = {0x0d,0x0a};
    fd = ComInit(path);
    if(fd == -1)
        return fd;
    else
    {
         res = _write(fd, buffer, strlen(buffer));
         res = _write(fd, end, 2);
         close(fd);
    }
    return res;
}
