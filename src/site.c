#include "../inc/core.h"
volatile int g_TotalSite = 0;
SPSITE pSite;
int ProtoAnalys(char *buf,int len)
{
    unsigned int crc = 0;
    int index = 0 ;
    char temp[16] = {0};
    char surplus[128] = {0};
    unsigned char pLen = 0;
    struct{
        char len[3];
        unsigned int crc;
        char type[16];
        char para1[32];
        char para2[32];
        char para3[32];
        char para4[32];
        char para5[32];
        unsigned char para_num;
    }sProt;
    memset(&sProt,0x00,sizeof(sProt));
    // #f06bed7b,19,GOTO,200.11,300.25
    if(*buf != '#' || len > 128)
        return 0;
    else
    {
        pLen = len-9;
        buf++;
        memcpy(temp,buf,8);
        buf += 8;
        sProt.crc = strtoul(temp,0,16);
        memcpy(surplus,buf,pLen);
        crc = crc32(surplus,pLen);
 //       printf("crc: %X,%X\n",crc,sProt.crc);
        if(crc == sProt.crc)
        {
            buf++;          // jump ,
            if(sscanf(buf,"%3[^,]",sProt.len) == 1)
            {
                if(sscanf(buf,"%*[^,],%16[^,]",sProt.type) == 1)
                {
                    if(sscanf(buf,"%*[^,],%*[^,],%32[^,]",sProt.para1) == 1)
                    {
                        sProt.para_num++;
                        if(sscanf(buf,"%*[^,],%*[^,],%*[^,],%32[^,]",sProt.para2) == 1)
                        {
                            sProt.para_num++;
                            if(sscanf(buf,"%*[^,],%*[^,],%*[^,],%*[^,],%32[^,]",sProt.para3) == 1)
                            {
                                sProt.para_num++;
                                if(sscanf(buf,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%32[^,]",sProt.para4) == 1)
                                    sProt.para_num++;
                            }
                        }
                    }
                    else
                        return 0;
                }
                else
                    return 0;
                if(strcmp(sProt.type,"GOTO") == 0)
                {
                    index = atoi(sProt.para3);
                    if((pSite+index)->have == 0)
                    {
                        (pSite+index)->have = 1;
                        g_TotalSite++;
                    }
                    printf("site index: %s\n",sProt.para3);
                }
                else if(strcmp(sProt.type,"CTRL") == 0)
                {
                    if(strcmp(sProt.para1,"STOP") == 0)
                        g_state.bit.gui = 1;
                    else if(strcmp(sProt.para1,"START") == 0)
                    {
                        g_state.bit.gui = 0;
                        g_state.bit.over = 0;
                    }
                }
            }
            else
                return 0;
        }
    }
    return 1;
}

int GuiInit(char* path)
{
    int i = 0;
    FILE *fp;
    unsigned long fsize;
    char temp[128] = { 0 };
    pSite = (SPSITE)malloc(sizeof(SSITE)*MAX_SITE);
    memset(pSite,0x00,sizeof(pSite));
    fsize = GetFSize(path);        // get file size
    if(fsize == 0)
    {
        lprintf(lfd,FATAL,"Site: File is empty!\n");
        return 0;
    }
    if((fp=fopen(path, "rb")) == NULL)
    {
        lprintf(lfd,FATAL,"Site: Not foud site file!\n");
        return 0;
    }
    for(i=0;;i++)
    {
        fgets(temp, 128, fp);
        sscanf(temp, "%lf %lf %63s",&(pSite+i)->x,&(pSite+i)->y,(pSite+i)->name);
        pSite->have = 0;
    //    printf("%s\n",(pSite+i)->name);
        memset(temp, 0x00, 128);
        if(fsize - ftell(fp) < 16)
            break;
    }
    return 1;
}

void SiteProcess(int arg)
{
    int i = 0;
    struct timeval tv;
    int tiger = 0;
    for(;;)
    {
        for(i=0; i<g_TotalSite; i++)
        {
            if((pSite+i)->have)
            {
                if(distOfPoint((pSite+i)->x, (pSite+i)->y , pGps->x, pGps->y) < 1.5)
                {
                    gettimeofday(&tv,NULL);
                    (pSite+i)->second = tv.tv_sec;
                    (pSite+i)->have = 0;
                    g_state.bit.arrive = 1;
                }
            }
        }
        for(i=0; i<g_TotalSite; i++)
        {
            if((pSite+i)->second)
            {
                gettimeofday(&tv,NULL);
                if((tv.tv_sec - (pSite+i)->second) > RESID_TIME)
                {
                    tiger = 0;
                    (pSite+i)->second = 0;
                }
                else
                    tiger = 1;
            }
        }
        if(tiger == 0)
            g_state.bit.arrive = 0;
        usleep(20000);
    }
}

