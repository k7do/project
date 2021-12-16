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
#include "textlcd.h"

static unsigned int timertoggle =0, timercount=0; //timertoggle: 타이머 모드 설정변수, timercount: 모드에 해당하는 초 설정
static unsigned int buzzertoggle=1, ledtoggle=1, fndtoggle=1, textlcdtoggle=1;//각 디바이스를 on/off하기 위한 변수로, 남은 버튼이 4개이기 때문에 led colorled를 통합, 기본은 1:on, 0:off


int main(int argc , char **argv)
{
    //각 드라이버 적재
    system("insmod ../buttondrv.ko");
    system("insmod ../buzzerdrv.ko");
    system("insmod ../fnddrv.ko");
    system("insmod ../leddrv.ko");
    system("insmod ../textlcddrv.ko");
    system("insmod ../colorleddrv.ko");

    //사용 변수 선언
    int ledFd, buzzerFd, buzzerEnableFd;
    BUTTON_MSG_T rxMsg, ledFd_temp,textlcd_temp;
    pthread_t buttonTh_id;

    //각 드라이버 init
    ledFd = ledInit();
    buzzerFd = buzzerInit(&buzzerEnableFd);
    buttonInit(&buttonTh_id);
    pwmLedInit();
    textlcdinit();
    
    ledFd_temp = ledFd;

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

        switch(rxMsg.keyInput)//버튼 값: 102=reset, 158=timer, 217=buzzertoggle, 139=ledtoggle, 115=fndtoggle, 114=textlcdtoggle
        {
            case 102: //리셋
                fndmode(s, 0); //fndmode(char mode, int fndnumber)
                textlcdmode(1, "RESET");
                textlcdmode(2,"        ");// textlcd 첫번째 라인에 reset표기
                ledOn(ledFd, 0x00);//ledOn(ledFd, 0xFF);
                pwmSetPercentRGB(0,0);
                pwmSetPercentRGB(0,1);//pwmSetPercentRGB(50,1);
                pwmSetPercentRGB(0,2);//pwmSetPercentRGB(50,2);
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 1);
                for(int i=0;i<0x1FFFFF;i++)//버저 플레이 시간 증가
                {}
                break;

            case 158: //change timer
                textlcdmode(1, "TIMER"); textlcdmode(2,"        ");
                switch(timertoggle){
                    case 0://timer 30s
                        textlcdmode(2, "30 sec");
                        timercount=30;//추후 사용할때fndmode(c, timercount);로 아마도 스레드 미사용시 카운트만 하는걸로 예상
                        timertoggle++;//모드 순차적 변경
                        ledOn(ledFd, 0x11);//의미없는 타이머모드 시간 차별성
                        pwmSetPercentRGB(30,0);
                        pwmSetPercentRGB(30,1);
                        pwmSetPercentRGB(30,2);
                        break;
                    case 1://timer 60s
                        textlcdmode(2, "60 sec");
                        timercount=60;//추후 사용할때fndmode(c, timercount);로 아마도 스레드 미사용시 카운트만 하는걸로 예상
                        timertoggle++;//모드 순차적 변경
                        ledOn(ledFd, 0x12);//의미없는 타이머모드 시간 차별성
                        pwmSetPercentRGB(60,0);
                        pwmSetPercentRGB(60,1);
                        pwmSetPercentRGB(60,2);
                        break;
                    default://timer 120s
                        textlcdmode(2, "120 sec");
                        timercount=120;//추후 사용할때fndmode(c,timercount);로 아마도 스레드 미사용시 카운트만 하는걸로 예상
                        timertoggle=0;//모드 순차적 변경
                        ledOn(ledFd, 0x13);//의미없는 타이머모드 시간 차별성
                        pwmSetPercentRGB(90,0);
                        pwmSetPercentRGB(90,1);
                        pwmSetPercentRGB(90,2);
                        break;
                }
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 2);
                for(int i=0;i<0x100000;i++)//버저 플레이 시간 증가
                {}
                break;

            case 217: //버저 onoff
                textlcdmode(1, "BUZZER_ON/OFF");
                textlcdmode(2,"        ");
                ledOn(ledFd, 0x00);//ledOn(ledFd, 0xFF);
                if(buzzertoggle==1) //기본상태=on:1 으로 실행되어서 버저를 끄는 함수
                {
                    textlcdmode(2,"  OFF   ");
                    buzzertoggle=0;// 반대로 끈다
                    write(buzzerEnableFd, &"0", 1); //buzzerInit(0)?
                    pwmSetPercentRGB(0,0);
                    pwmSetPercentRGB(0,1);
                    pwmSetPercentRGB(0,2);
                }
                else{
                    textlcdmode(2,"  ON    ");
                    buzzertoggle=1;//buzzer on
                    buzzerInit(&buzzerEnableFd);
                    for(int buzzerpoweron = 1; buzzerpoweron < 9; buzzerpoweron++)
                    {
                        buzzerPlaySong(buzzerFd, buzzerEnableFd, buzzerpoweron); //on상태인지 확인?
                        for(int i=0;i<0x100000;i++)//버저 플레이 시간 증가
                            {}
                    }
                    pwmSetPercentRGB(50,0);
                    pwmSetPercentRGB(50,1);
                    pwmSetPercentRGB(50,2);
                }
                break;

            case 139: //led on/off
                textlcdmode(1, "LED_ON/OFF");
                textlcdmode(2,"        ");// textlcd 첫번째 라인에 reset표기
                if(ledtoggle ==1 )//led,colorled on->off
                {
                    ledtoggle=0;
                    pwmInactiveAll();//colorled off
                    ledFd = 0;//ledExit()와 같은 기능?
                    pwmSetPercentRGB(0,0);
                    pwmSetPercentRGB(0,1);
                    pwmSetPercentRGB(0,2);
                }
                else//led,colorled off->on
                {
                    ledtoggle =1;
                    ledFd = ledFd_temp;
                    pwmActiveAll();
                    pwmSetPercentRGB(50,0);
                    pwmSetPercentRGB(50,1);
                    pwmSetPercentRGB(50,2);
                }
                buzzerPlaySong(buzzerFd, buzzerEnableFd,3);
                for(int i=0;i<0x1FFFFF;i++)//버저 플레이 시간 증가
                {}
                break;

            case 115: //fnd on/off
                textlcdmode(1, "FND_ON/OFF");
                textlcdmode(2,"        ");// textlcd 첫번째 라인에 reset표기
                if(fndtoggle==1)
                {
                    fndtoggle=0;
                    fndonoff=0;
                    pwmSetPercentRGB(0,0);
                    pwmSetPercentRGB(0,1);
                    pwmSetPercentRGB(0,2);
                }
                else
                {
                    fndtoggle=1;
                    fndonoff=1;
                    pwmSetPercentRGB(50,0);
                    pwmSetPercentRGB(50,1);
                    pwmSetPercentRGB(50,2);
                }
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 4);
                for(int i=0;i<0x1FFFFF;i++)//버저 플레이 시간 증가
                {}
                break;


            default: //114값 해당, textlcd on/off
                textlcdmode(1, "TEXTLCD_ON/OFF");
                textlcdmode(2,"        ");
                //ledOn(ledFd, 0x00);
                if(textlcdtoggle==1)
                {
                    textlcdtoggle =0;
                    textlcdexit();
                    pwmSetPercentRGB(0,0);
                    pwmSetPercentRGB(0,1);
                    pwmSetPercentRGB(0,2);
                }
                else
                {
                    textlcdtoggle =1;
                    textlcdinit();
                    pwmSetPercentRGB(50,0);
                    pwmSetPercentRGB(50,1);
                    pwmSetPercentRGB(50,2);
                }
                buzzerPlaySong(buzzerFd, buzzerEnableFd, 5);
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
    textlcdexit();
    //각 적재형 드라이버 제거
    system("rmmod ../buttondrv.ko");
    system("rmmod ../buzzerdrv.ko");
    system("rmmod ../fnddrv.ko");
    system("rmmod ../leddrv.ko");
    system("rmmod ../textlcddrv.ko");

    return 0;
}

//led, Button Complete;