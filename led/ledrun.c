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

#include "led.h"
#define LED_DRIVER_NAME "/dev/periled"

void doHelp(void)
{
    printf("ledrun <hex byte> :data bit0 operation 1=>on 0=>off\n");
    printf(" ledrun 0x05 ;4th and 1th led on\n");
    printf(" ledrun 0xff ;all led on\n");
    printf(" ledrun 0x00 ;all led off\n");
}
int main(int argc , char **argv)
{
    unsigned int data = 0;
    if (argc < 2 )
    {
        perror(" Args number is less than 2\n");
        doHelp();
        return 1;
    }

    data = strtol(argv[1],NULL,16);
    printf("write data :0x%X\n", data);
    ledLibInit();
    ledOn(data);
    return 0;
}
