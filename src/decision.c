#include "../inc/core.h"

//FILE* fps;
void kernel(void)
{
    char latlon[48] = {0};
    int angle = 0;
    int temp = 0;
    unsigned char bit = 0;
    double x; //current x coordinate value
    double y; //current y corrdinate value
    if(initiateTrack(track_path)==0)
        goto end;
    //   PSELF pSelf = (PSELF)malloc(sizeof(SELF));
    PCOOR pCoor = (PCOOR)malloc(sizeof(COOR));
    for(;;)
    {
        if(pGps->status==5 || pGps->status==0x0b)
        {
            FILE *fp = fopen(out_path,"w+");
            while(1)
            {
                if(pGps->status==5 || pGps->status==0x0b)
                {
                    if(!bit)
                        lprintf(lfd, INFO, "GPS: RTK signal normal.");
                    bit = 1;
                    g_state.bit.signal = 0;
                    WGS84ToGuass(pGps->latitude,pGps->longitude, &x, &y);
                    pGps->x = x;
                    pGps->y = y;
                    pCoor->cur_x = x;
                    pCoor->cur_y = y;
                    pCoor->cur_azimuth = pGps->azimuth;
                    pCoor->speed = pGps->vn;
                    pCoor->cur_index = 0;
                    if(total-(pCoor->n_index) < 5)
                    {
                        if(g_state.bit.over == 0)
                            lprintf(lfd, INFO, "Kernel: autopilot end!");
                        g_state.bit.over = 1;
                    }
                    temp = followTrack(pCoor);
                    if(temp != 4000)
                        angle = (STEER_CENTER_VALUE - temp);
                    else
                    {
                        if(g_state.bit.start == 0)
                            lprintf(lfd, FATAL,"Kernel: Overmigration of starting position!");
                        g_state.bit.start = 1;
                        angle = STEER_CENTER_VALUE;
                    }
                    //_ker_printf("angle: %d\n",ccpParam->angle);
                    snprintf(latlon,47,"%f %f\n",x,y);
                    fputs(latlon,fp);
                    memset(latlon,0x00,48);
                }
                else
                {
                    if(bit)
                    {
                        lprintf(lfd, WARN, "GPS: RTK signal abnormal!");
                        bit = 0;
                    }
                    g_state.bit.signal = 1;
                }
                VehCtrl(g_state.state,angle);
                usleep(20000);	// 延时20ms
            }
            fclose(fp);
        }
        else
        {
            if(RtkSet(rtk_path) == -1)
                lprintf(lfd,FATAL,"GPS: RTK setting failed!");
        }
        sleep(2);
    }
    free(pCoor);
end:
    pthread_exit((void*)1);
    sleep(1);
}
