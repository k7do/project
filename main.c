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

//사용할 헤더파일
#include "led.h"
#include "button.h"
#include "buzzer.h"
#include "colorled.h"
#include "fnd.h"    


int main(int argc , char **argv)
{
    //각 드라이버 적재
    system("insmod ../buttondrv.ko");
    system("insmod ../buzzerdrv.ko");
    system("insmod ../fnddrv.ko");
    system("insmod ../leddrv.ko");
    system("insmod ../textlcddrv.ko");
    system("insmod ../fnddrv.ko");

    //사용 변수 선언
    int ledFd, buzzerFd, buzzerEnableFd;
    BUTTON_MSG_T rxMsg;
    pthread_t buttonTh_id;

    //각 드라이버 init
    ledFd = ledInit();
    buzzerFd = buzzerInit(&buzzerEnableFd);
    buttonInit(&buttonTh_id);
    pwmLedInit();

    //RGB percent 0으로 보여주고 50퍼센트로 보여주기
    pwmSetPercentRGB(0,0);
    pwmSetPercentRGB(0,1);
    pwmSetPercentRGB(0,2);
    pwmSetWave(50);// 50주기로 rgb를 차례대로 서서히 onoff

    //msg받아오고 실패시 리턴
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
                fndmode(s, 102); //fndmode(char mode, int fndnumber)
                ledOn(ledFd, 0xFF);
                pwmSetPercentRGB(0,0);
                pwmSetPercentRGB(50,1);
                pwmSetPercentRGB(50,2);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 1);
                for(int i=0;i<0xFFFFFF;i++)//버저 플레이 시간 증가
                {}
                break;

            case 158: 
                fndmode(s, 158);
                ledOn(ledFd, 0x0F);
                pwmSetPercentRGB(50,0);
                pwmSetPercentRGB(0,1);
                pwmSetPercentRGB(50,2);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 8);
                for(int i=0;i<0xFFFFFF;i++)//버저 플레이 시간 증가
                {}
                break;

            default:
                fndmode(s, 0);
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

    //각 디바이스 종료 
    ledExit(ledFd);
    buzzerExit(buzzerEnableFd, buzzerFd);
    fndOff();
    //각 적재형 드라이버 제거
    system("rmmod ../buttondrv.ko");
    system("rmmod ../buzzerdrv.ko");
    system("rmmod ../fnddrv.ko");
    system("rmmod ../leddrv.ko");
    system("rmmod ../textlcddrv.ko");

    return 0;
}

//led, Button Complete;