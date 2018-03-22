#ifndef SITE_H
#define SITE_H

typedef struct sit{
    double x;
    double y;
    int index;
    char name[64];
    unsigned char have;
    long second;
}SSITE,*SPSITE;

extern int ProtoAnalys(char *buf,int len);
extern int GuiInit(char* path);
extern void SiteProcess(int arg);

extern SPSITE pSite;
extern volatile int g_TotalSite;

#define MAX_SITE 100
#define RESID_TIME  50
#endif
