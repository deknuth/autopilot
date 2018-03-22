#include "../inc/core.h"
int output = 0;
/*
struct ctrl{
    unsigned char over  :1;
    unsigned char signal :1;
    unsigned char obstacle :1;
    unsigned char gui :1;
    unsigned char arrive :1;
    unsigned char  :3;
};
*/
void VStop(void)
{
    ccpParam->state = STOP;
    ccpParam->angle = STEER_CENTER_VALUE;
    ccpParam->throttle = 0;
    ccpParam->gear = 0;
    ccpParam->brake = 250;
    _ker_printf("Vehicle stop!\n");
}

void VRun(int angle)
{
    ccpParam->state = START;
    ccpParam->angle = angle;
    ccpParam->throttle = 1900;
    ccpParam->gear = 1;
    ccpParam->brake = 0;
}

void VPause(void)
{
    ccpParam->state = PAUSE;
    ccpParam->throttle = 0;
    ccpParam->brake = 250;
    _ker_printf("Vehicle pause!\n");
}

void VehCtrl(unsigned char state,int angle)  //  Vehicle parameter control
{
    if(state & 0x01)
        VStop();
    else if(state == 0)
        VRun(angle);
    else
        VPause();
}

