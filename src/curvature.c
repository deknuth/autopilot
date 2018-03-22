#include "../inc/core.h"

typedef struct cc{
    double  x1;
    double  y1;
    double  a1;
    double  x2;
    double  y2;
    double  a2;
    double  x3;
    double  y3;
    double  a3;
}*PCURVA,CURVA;         // Curvature struct

double cacuCurvature(PCURVA pCurva)
{
    double K = 0;
    double S = 0;
    double L = 0;
    double a,b,c,d,e,f;

    pCurva->x1 = pCurva->x1 + delta_x(pCurva->a1); //longitude
    pCurva->y1 = pCurva->y1 + delta_y(pCurva->a1); //latitude

    pCurva->x2 = pCurva->x2 + delta_x(pCurva->a2); //longitude
    pCurva->y2 = pCurva->y2 + delta_y(pCurva->a2); //latitude

    pCurva->x3 = pCurva->x3 + delta_x(pCurva->a3); //longitude
    pCurva->y3 = pCurva->y3 + delta_y(pCurva->a3); //latitude

    a = pCurva->y2 - pCurva->y1;
    b = pCurva->x2 - pCurva->x1;
    c = pCurva->y3 - pCurva->y1;
    d = pCurva->x3 - pCurva->x1;
    e = pCurva->y2 - pCurva->y3;
    f = pCurva->x2 - pCurva->x3;
    S=((pCurva->x2 - pCurva->x1)*(pCurva->y3 - pCurva->y1)-(pCurva->x3-pCurva->x1)*(pCurva->y2-pCurva->y1))/2;// triangle area
    L=(sqrt(pow((double) fabs(a),2.0)+pow((double) fabs(b),2.0))*(sqrt(pow((double) fabs(c),2.0)+pow((double) fabs(d),2.0)))*(sqrt(pow((double) fabs(e),2.0)+pow((double) fabs(f),2.0))));//L=A*B*C
    if(L == 0)
        return 0;
    else
    {
        K=(4*S)/L;      //curvature
        return K;
    }
}
