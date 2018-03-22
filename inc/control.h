#ifndef CONTROL_H
#define CONTROL_H
void VehCtrl(unsigned char state,int angle);
extern int output;

struct ctrl{
    unsigned char over :1;
    unsigned char signal :1;
    unsigned char obstacle :1;
    unsigned char gui :1;
    unsigned char arrive :1;
    unsigned char start :1;
    unsigned char  :2;
};

typedef union {
    struct ctrl bit;
    unsigned char state;
}USTATE;

extern USTATE g_state;
#endif // CONTROL_H
