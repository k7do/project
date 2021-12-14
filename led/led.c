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

int ledOn(int fd, int ledNum)
{
    write (fd, &ledNum, 4);
}

int ledInit(void)
{
    int fd;
    fd=open(LED_DRIVER_NAME, O_WRONLY);
    return fd;
}

int ledExit(int fd)
{
    close(fd);
}
