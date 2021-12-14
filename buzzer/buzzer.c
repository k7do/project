#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"

const int musicScale[MAX_SCALE_STEP] =
{
    262, /*do*/ 294, 330, 349, 392, 440, 494, /* si */ 523
};

int buzzerInit(int *enableFd)
{
    DIR *dir_info;

    char gBuzzerBaseSysDir[128];
    dir_info = opendir(BUZZER_BASE_SYS_PATH);
    int ifNotFound = 1;
    if (dir_info != NULL)
    {
        while (1)
        {
            struct dirent *dir_entry;
            dir_entry = readdir(dir_info);
            if (dir_entry == NULL)
                break;
            if (strncasecmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0)
            {
                ifNotFound = 0;
                sprintf(gBuzzerBaseSysDir, "%s%s/", BUZZER_BASE_SYS_PATH, dir_entry->d_name);
            }
        }
    }
    printf("find %s\n", gBuzzerBaseSysDir);

    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_ENABLE_NAME);
    *enableFd = open(path, O_WRONLY);

    sprintf(path,"%s%s",gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    int fd=open(path,O_WRONLY);

    return fd;
}

int buzzerPlaySong(int fd, int enableFd, int scale)
{
    if (scale > MAX_SCALE_STEP)
    {
        printf(" <buzzerNo> over range \n");
        doHelp();
        return 1;
    }

    else
    {
        dprintf(fd, "%d", musicScale[scale - 1]);
        write(enableFd, &"1", 1);
    }
}

void buzzerExit(int enableFd, int fd)
{
    write(enableFd, &"0", 1);
    close(enableFd);
    close(fd);
    //close(dir_info);
}

void doHelp(void)
{
    printf("Usage:\n");
    printf("buzzertest <buzzerNo> \n");
    printf("buzzerNo: \n");
    printf("do(1),re(2),mi(3),fa(4),sol(5),ra(6),si(7),do(8) \n");
    printf("off(0)\n");
}