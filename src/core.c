#include "../inc/core.h"
//char temp[48] = {"/home/lovenix/data/out_%s.txt"};
log_t *lfd;
char track_path[128] = {0};
char out_path[128] = {0};
char rtk_path[128] = {0};
int gfd;        // GPS com handle
sem_t sem_cs;
tpool_t *pool;
USTATE g_state;

void quit(int a)
{
    printf("out: %d !\n",a);
    close(gfd);
    exit(0);
}

int main(void)
{
    char *env;
    void *a = 0;
    int res = 0;
    char gps_path[48] = {0};
    char site_path[48] = {0};
    g_state.state = 0;
    env = getenv("AT_PATH");
    if(env == NULL)
    {
        printf("Please check the environment variable!\n");
        exit(0);
    }
#if 0
    if(argc != 3)
    {
        printf("Usage: ./auto track/track.txt out/out.txt\n");
        exit(0);
    }
    else
    {
        memcpy(track_path,argv[1],strlen(argv[1]));
        //sprintf(out_path,temp,argv[2]);
        memcpy(out_path,argv[2],strlen(argv[2]));
 //       printf("%s\n",track_path);
 //       printf("%s\n",out_path);
    }
#endif
    pthread_t id1,id2,id3,id4,id5,id6,id7;
    sem_init(&sem_cs, 0, 0);

    if((lfd = log_open(0)) == 0)
    {
        printf("Kernel: Open log file failed!");
        exit(0);
    }
    res = ProfileCheck();
    if(res == 0)
        DefaultConfigSet();
    else if(res == -1)
        exit(0);
    else
    {
        GetConfigArg(3, gps_path);
        GetConfigArg(4, rtk_path);
        GetConfigArg(5, track_path);
        GetConfigArg(6, site_path);
        CreatOutFile(out_path,env);
        printf("track: %s\n",track_path);
        printf("gps: %s\n",gps_path);
        printf("rtk: %s\n",rtk_path);
        printf("out: %s\n",out_path);
        printf("site: %s\n",site_path);
    }
    GuiInit(site_path);

    signal(SIGINT, quit);		// ctrl+C
#ifdef  PRINT
    if(CreatPipe() == 0)
    {
        _hal_printf("Pipe create falied!\n");
        exit(0);
    }
#endif
    /********** RTK PART **********/
    if(RtkSet(rtk_path) == -1)
        lprintf(lfd,FATAL,"GPS: RTK setting failed!");
    sleep(1);

    /********** CAN PART **********/
    int i = 0;
    if(InitCAN(VCI_USBCAN2) == 0)		// Initialization device
        exit(0);
    GetCANInfo(VCI_USBCAN2);			// get device information
    if(pthread_create(&id1,NULL,(void *)ReceiveFrame,0) != 0)
         lprintf(lfd,FATAL,"Thread: Create ReceiveFrame thread failed!");
    frame = (PVCI_CAN_OBJ)malloc(sizeof(VCI_CAN_OBJ));
    ccpParam = (PAP_PARAM)malloc(sizeof(AP_PARAM));
    memset(frame,0x00,sizeof(VCI_CAN_OBJ));
    memset(ccpParam,0x00,sizeof(AP_PARAM));
    InitFrame(frame);
    frame->Data[0] = REQ_CONTROL;		//request control

    for(i=0; i<10; i++)
    {
        if(ctrl_flag)
        {
            ctrl_flag = 0;
            break;
        }
        SendCTLFrame(frame);
        usleep(100000);	// 延时100ms
    }
    if(i < 10)
    {
        ControlInit();
        usleep(10000);	// delay 10ms
        if(pthread_create(&id2,NULL,(void *)ControlFrame,(void *)0) != 0)
        {
            lprintf(lfd,FATAL,"Thread: Create ControlFrame thread failed!");
            _hal_printf("pthread_create ControlFrame falied!\n");
        }
    }
    else
    {
        _hal_printf("Get control fault!\n");
        lprintf(lfd,FATAL,"CAN: Get control failed!");
        CloseCAN(frame);
        exit(0);
    }
    pGps = (PGPS)malloc(sizeof(GPSINFO));
    /************************** GPS PART ************************/
    if((gfd = ComInit(gps_path)) == -1)
    {
        lprintf(lfd,FATAL,"GPS: GPS port open failed!");
        goto end;
    }
    if(pthread_create(&id3,NULL,(void *)GpsProcess,(void *)0) != 0)
        lprintf(lfd,FATAL,"Thread: Create GpsProcess thread failed!");
    sleep(2);

    /********************** decision PART ***********************/
    if(pthread_create(&id4,NULL,(void *)kernel,0) != 0)
        lprintf(lfd,FATAL,"Thread: Create kernel thread failed!");

    /********************** lidar PART ***********************/
    if(pthread_create(&id5,NULL,(void *)LidarClient,(void *)0) != 0)
        lprintf(lfd,FATAL,"Thread: Create LidarClient thread failed!");
    usleep(2000);
    if(pthread_create(&id6,NULL,(void *)LidarCluster,(void *)0) != 0)
        lprintf(lfd,FATAL,"Thread: Create LidarCluster thread failed!");

    /********************** socket PART ***********************/
    pool = tpool_init(64, 64, 1);

    if(pthread_create(&id7,NULL,(void *)TcpServer,(void *)TCP_PORT) != 0)
        lprintf(lfd,FATAL,"Thread: Create TcpServer thread failed!");
    tpool_add_work(pool, SiteProcess, 0);
#if 1
    pthread_join(id1,NULL);
    pthread_join(id2,NULL);
    pthread_join(id3,NULL);
    pthread_join(id4,a);
    pthread_join(id5,NULL);
    pthread_join(id6,NULL);
#endif
    pthread_join(id7,NULL);
    tpool_destroy(pool, 1);
end:
    if(*(int*)a == 0)
        free(pTrack);
    CloseCAN(frame);
    sem_destroy(&sem_cs);
    free(pGps);
    log_close(lfd);
    return 1;
}

