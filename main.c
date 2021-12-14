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
#include "colorled.h"

int main(int argc , char **argv)
{
    system("insmod ../buttondrv.ko");
    system("insmod ../buzzerdrv.ko");
    system("insmod ../fnddrv.ko");
    system("insmod ../leddrv.ko");
    system("insmod ../textlcddrv.ko");

    int ledFd, buzzerFd, buzzerEnableFd;
    BUTTON_MSG_T rxMsg;
    pthread_t buttonTh_id;

    ledFd = ledInit();
    buzzerFd = buzzerInit(&buzzerEnableFd);
    buttonInit(&buttonTh_id);
    pwmLedInit();

    pwmSetPercentRGB(0,0);
    pwmSetPercentRGB(0,1);
    pwmSetPercentRGB(0,2);

    int msgQueue = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    if (msgQueue == -1)
    {
        printf ("Cannot get msgQueueID, Return!\r\n");
        return -1;
    }

    while (1)
    {
        int returnValue = 0;
        returnValue = msgrcv(msgQueue, &rxMsg, sizeof(BUTTON_MSG_T), 0, IPC_NOWAIT);
        if (returnValue == -1)
            break; //There is no message at all
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
                pwmSetPercentRGB(0,0);
                pwmSetPercentRGB(50,1);
                pwmSetPercentRGB(50,2);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 1);
                break;

            case 158: 
                ledOn(ledFd, 0x0F);
                pwmSetPercentRGB(50,0);
                pwmSetPercentRGB(0,1);
                pwmSetPercentRGB(50,2);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 8);
                break;

            default:
                ledOn(ledFd, 0x00);
                write(buzzerEnableFd, &"0", 1);
                pwmSetPercentRGB(0,0);
                pwmSetPercentRGB(0,1);
                pwmSetPercentRGB(0,2);
                break;
        }

        if(rxMsg.keyInput == 114 && rxMsg.pressed == 0)
        {
            pthread_exit(NULL);
        }
    }        

    ledExit(ledFd);
    buzzerExit(buzzerEnableFd, buzzerFd);

    system("rmmod ../buttondrv.ko");
    system("rmmod ../buzzerdrv.ko");
    system("rmmod ../fnddrv.ko");
    system("rmmod ../leddrv.ko");
    system("rmmod ../textlcddrv.ko");

    return 0;
}

//led, Button Complete;