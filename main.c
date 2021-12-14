#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include "led.h"
#include "button.h"
#include "buzzer.h"

int main(int argc , char **argv)
{
    int ledFd, buzzerFd, buzzerEnableFd;
    BUTTON_MSG_T rxMsg;

    ledFd = ledInit();
    buzzerFd = buzzerInit(&buzzerEnableFd);
    buttonInit();

    int msgQueue = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    if (msgQueue == -1)
    {
        printf ("Cannot get msgQueueID, Return!\r\n");
        return -1;
    }
    
    while (1)
    {
        int returnValue;
        returnValue = msgrcv(msgQueue, &rxMsg, sizeof(BUTTON_MSG_T), 0, 0);
        printf("key input: %d, key pressed?: %d\r\n", rxMsg.keyInput, rxMsg.pressed);

        switch(rxMsg.keyInput)
        {
            case 102: 
                ledOn(ledFd, 0xFF);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 1);
                break;

            case 158: 
                ledOn(ledFd, 0x00);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 8);
                break;

            default:
                write(buzzerEnableFd, &"0", 1);
                break;
        }
    }         

    ledExit(ledFd);
    buzzerExit(buzzerEnableFd, buzzerFd);
    return 0;
}

//led, Button Complete;