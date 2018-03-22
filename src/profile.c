/*
 * profile.c
 *
 *  Created on: 2018-3-16
 *      Author: lovenix
 */
#include "../inc/core.h"

int DefaultConfigSet(void)			// 生成默认配置文件
{
	FILE* fp;
	if((fp = fopen(CONFIG_FILE, "ab")) == NULL)
		return 0;
    fprintf(fp, "%s", "GUI_PORT\t1111\t\r\n");
    fprintf(fp, "%s", "LIDAR_IP\t192.168.1.111\t\r\n");
    fprintf(fp, "%s", "GPS_PORT\t/dev/ttyUSB0\t\r\n");
    fprintf(fp, "%s", "RTK_PORT\t/dev/ttyUSB1\t\r\n");
    fprintf(fp, "%s", "TRACK_FILE\ttrack/track_f.txt\t\r\n");
    fprintf(fp, "%s", "SITE_FILE\ttrack/site\t\r\n");
    fprintf(fp, "%s", "CLI_OUT_TIME\t15\t\r\n");
	fclose(fp);
	return 1;
}

int GetConfigArg(int num, char* buf)		// 获取某行配置参数
{
    int i,res;
    char *p = NULL;
	FILE* fp;
    char temp[48] = {0};
	fp = fopen(CONFIG_FILE, "rb");
	for (i = 0; i < num - 1; i++)
	{
        p = fgets(&temp[0], 48, fp);
        res = ftell(fp);                    // 设置文件偏移量
		if (feof(fp) != 0)
			break;
	}
	i = 0;
    memset(temp, 0x00, 48);
    p = fgets(&temp[0], 48, fp);
	while (temp[i++] != 0x09)
    {
        if(i > 47)
        {
            fclose(fp);
            return 0;
        }
    }
	while (temp[i] != 0x09)
    {
		*(buf++) = temp[i++];
        if(i > 47)
        {
            fclose(fp);
            return 0;
        }
    }
	*buf = '\0';
	fclose(fp);
    return 1;
}

int ConfigModif(int num, char* con)		// 修改配置文件
{
    int i, j ,res=0;
    char *p = NULL;
	FILE* fp;
	FILE* ftemp;
	char temp[32] = { 0 };
	char head[5] = { 0 };
	fp = fopen(CONFIG_FILE, "rb");
	ftemp = tmpfile();
	for (i = 0; i < num - 1; i++)
	{
        p = fgets(&temp[0], 32, fp);
        res = ftell(fp);                    // 设置文件偏移量
		fprintf(ftemp, "%s", temp);
		memset(temp, 0x00, 32);
		if (feof(fp) != 0)
			break;
	}
    p = fgets(&temp[0], 32, fp);
	j = i = 0;
	while (temp[i] != 0x09)
		head[j++] = temp[i++];
	head[j] = '\0';
	memset(temp, 0x00, 32);
	snprintf(temp, sizeof(temp), "%s%s%s%s", head, "\t", con, "\t\r\n");
	fprintf(ftemp, "%s", temp);
	while (1)
	{
        p = fgets(&temp[0], 32, fp);
        res = ftell(fp);                    // 设置文件偏移量
		fprintf(ftemp, "%s", temp);
		memset(temp, 0x00, 32);
		if (feof(fp) != 0)
			break;
	}
	fclose(fp);
	if (remove(CONFIG_FILE) == -1)
		lprintf(lfd, FATAL, "Profile: Remove configure file error!");
	if ((fp = fopen(CONFIG_FILE, "w")) == NULL)
		lprintf(lfd, FATAL, "Profile: Create new file error!");
	if (fseek(ftemp, 0, SEEK_SET) != 0)
		lprintf(lfd, FATAL, "Profile: lseek error!");
	while (1)
	{
        p = fgets(&temp[0], 32, ftemp);
        res = ftell(ftemp);                    // 设置文件偏移量
		fprintf(fp, "%s", temp);
		memset(temp, 0x00, 32);
		if (feof(ftemp) != 0)
			break;
	}
	fclose(ftemp);
	fclose(fp);
	return (1);
}

int ProfileCheck(void)   		// 检测配置文件是否存在
{
	int fd;
	struct stat buf;
    if ((fd = open(CONFIG_FILE, O_RDONLY)) == -1)
	{
        if((fd = open(CONFIG_FILE, O_RDONLY | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR)) == -1)		// create file
		{
            lprintf(lfd,FATAL,"Profile: Open configure file failed!");
            return -1;
		}
        else
            close(fd);
	}
	fstat(fd, &buf);
	if (buf.st_size == 0)
		return 0;
	else
		return 1;
}

