#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define LED_DRIVER_NAME "/dev/periled"
#include "led.h"

static int fd = 0;
int ledLibInit(void);
int ledOn(int ledNum);
int ledLibExit(void);

int ledOn(int ledNum)
{
    write (fd, &ledNum, 4);
}

int ledLibInit(void)
{
    fd=open(LED_DRIVER_NAME, O_WRONLY);
}

int ledLibExit(void)
{
    close(fd);
}
