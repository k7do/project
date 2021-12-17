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
#include <sys/kd.h>

//사용할 헤더파일
#include "led.h"
#include "button.h"
#include "buzzer.h"
#include "colorled.h"
#include "fnd.h"
#include "textlcd.h"
#include "touch.h"
#include "libbmp.h"
#include "libfbdev.h"

static unsigned int timertoggle = 0, timercount = 0, timCount = 0;                                   //timertoggle: 타이머 모드 설정변수, timercount: 모드에 해당하는 초 설정
static unsigned int buzzertoggle = 1, ledtoggle = 1, fndtoggle = 1, textlcdtoggle = 1; //각 디바이스를 on/off하기 위한 변수로, 남은 버튼이 4개이기 때문에 led colorled를 통합, 기본은 1:on, 0:off

typedef enum
{
    MAIN = 0,
    BOARD = 1,
    EXIT = 2
} SCREENTYPE;

int main(int argc, char **argv)
{
    //각 드라이버 적재
    system("insmod ../buttondrv.ko");
    system("insmod ../buzzerdrv.ko");
    system("insmod ../fnddrv.ko");
    system("insmod ../leddrv.ko");
    system("insmod ../textlcddrv.ko");
    system("insmod ../colorleddrv.ko");

    int conFD = open("/dev/tty0", O_RDWR);
    ioctl(conFD, KDSETMODE, KD_GRAPHICS);
    close(conFD);

    //사용 변수 선언
    int ledFd, buzzerFd, buzzerEnableFd, ledFd_temp, textlcd_temp;
    SCREENTYPE screenType = MAIN;

    int tick_on = 0;
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    int timeResult;
    int op_cols = 0, op_rows = 0;
    char *op_data;

    int board_cols = 0, board_rows = 0;
    char *board_data;

    TOUCH_MSG_T rcvMsg;
    BUTTON_MSG_T rxMsg;
    pthread_t buttonTh_id;
    pthread_t touchTh_id;

    time_t prevTime, curTime;
    //각 드라이버 init
    ledFd = ledInit();
    buzzerFd = buzzerInit(&buzzerEnableFd);
    buttonInit(&buttonTh_id);
    pwmLedInit();
    textlcdinit();
    touchInit(&touchTh_id);

    ledFd_temp = ledFd;

    //RGB percent 0으로 보여주고 50퍼센트로 보여주기
    pwmSetPercentRGB(0, 0);
    pwmSetPercentRGB(0, 1);
    pwmSetPercentRGB(0, 2);
    pwmSetWave(50); // 50주기로 rgb를 차례대로 서서히 onoff

    if (fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0)
    {
        printf("FrameBuffer Init Failed\r\n");
        return 0;
    }

    //FileRead
    if (read_bmp("./option.bmp", &op_data, &op_cols, &op_rows) < 0)
    {
        printf("File open failed\r\n");
        return 0;
    }

    if (read_bmp("./aa.bmp", &board_data, &board_cols, &board_rows) < 0)
    {
        printf("File open failed\r\n");
        return 0;
    }

    fb_clear();
    fb_write(op_data, op_cols, op_rows);

    //FileRead
    //fb_write(board_data, board_cols, board_rows);

    //msg받아오고 실패시 리턴
    int msgQueue = msgget(B_MESSAGE_ID, IPC_CREAT | 0666);
    if (msgQueue == -1)
    {
        printf("Cannot get B_msgQueueID, Return!\r\n");
        return -1;
    }

    int msgID = msgget(T_MESSAGE_ID, IPC_CREAT | 0666);
    if (msgID == -1)
    {
        printf("Cannot get T_msgQueueID, Return!\r\n");
        return -1;
    }

    while (1)
    {
        int B_returnValue = 0;
        B_returnValue = msgrcv(msgQueue, &rxMsg, sizeof(BUTTON_MSG_T) - sizeof(long int), 0, IPC_NOWAIT);
        if (B_returnValue == -1)
            break; //There is no message at all
    }

    while (1)
    {
        int T_returnValue = 0;
        T_returnValue = msgrcv(msgID, &rcvMsg, sizeof(rcvMsg) - sizeof(long int), 0, IPC_NOWAIT);
        if (T_returnValue == -1)
            break; //There is no message at all
    }

    printf("main ready\r\n");

    while (1)
    {
        if(tick_on && (time(NULL) - prevTime >= 1))
        {
            timCount--;

            fndmode('s', timCount);

            if(timCount <= 0)
                tick_on = 0;
        }

        prevTime = time(NULL);

        int B_returnValue = 0;
        int T_returnValue = 0;

        B_returnValue = msgrcv(msgQueue, &rxMsg, sizeof(BUTTON_MSG_T) - sizeof(long int), 199, IPC_NOWAIT);
        T_returnValue = msgrcv(msgID, &rcvMsg, sizeof(rcvMsg) - sizeof(long int), 200, IPC_NOWAIT);


        if (screenType == EXIT)
            break;

        if (rxMsg.pressed == 1)
        {
            printf("key input: %d, key pressed?: %d\r\n", rxMsg.keyInput, rxMsg.pressed);
            switch (rxMsg.keyInput) //버튼 값: 102=reset, 158=timer, 217=buzzertoggle, 139=ledtoggle, 115=fndtoggle, 114=textlcdtoggle
            {
                case 102:            //리셋
                    timCount = 0;
                    fndmode('s', 0); //fndmode(char mode, int fndnumber)
                    textlcdmode(1, "RESET");
                    textlcdmode(2, "        "); // textlcd 첫번째 라인에 reset표기
                    ledOn(ledFd, 0x00);         //ledOn(ledFd, 0xFF);
                    pwmSetPercentRGB(0, 0);
                    pwmSetPercentRGB(0, 1); //pwmSetPercentRGB(50,1);
                    pwmSetPercentRGB(0, 2); //pwmSetPercentRGB(50,2);
                    break;

                case 158: //change timer
                    textlcdmode(1, "TIMER");
                    textlcdmode(2, "        ");
                    switch (timertoggle)
                    {
                    case 0: //timer 30s
                        textlcdmode(2, "30 sec");
                        timercount = 30;    //추후 사용할때fndmode(c, timercount);로 아마도 스레드 미사용시 카운트만 하는걸로 예상
                        timertoggle++;      //모드 순차적 변경
                        ledOn(ledFd, 0x11); //의미없는 타이머모드 시간 차별성
                        break;
                    case 1: //timer 60s
                        textlcdmode(2, "60 sec");
                        timercount = 60;    //추후 사용할때fndmode(c, timercount);로 아마도 스레드 미사용시 카운트만 하는걸로 예상
                        timertoggle++;      //모드 순차적 변경
                        ledOn(ledFd, 0x12); //의미없는 타이머모드 시간 차별성
                        break;
                    default: //timer 120s
                        textlcdmode(2, "120 sec");
                        timercount = 120;   //추후 사용할때fndmode(c,timercount);로 아마도 스레드 미사용시 카운트만 하는걸로 예상
                        timertoggle = 0;    //모드 순차적 변경
                        ledOn(ledFd, 0x14); //의미없는 타이머모드 시간 차별성
                        break;
                    }
                    break;

                case 217: //버저 onoff
                    textlcdmode(1, "BUZZER_ON/OFF");
                    if (buzzertoggle == 1) //기본상태=on:1 으로 실행되어서 버저를 끄는 함수
                    {
                        textlcdmode(2, "  OFF   ");
                        buzzertoggle = 0;               // 반대로 끈다
                        write(buzzerEnableFd, &"0", 1); //buzzerInit(0)?
                        
                    }
                    else if (buzzertoggle == 0)
                    {
                        textlcdmode(2, "  ON    ");
                        buzzertoggle = 1; //buzzer on
                        buzzerInit(&buzzerEnableFd);
                        for (int buzzerpoweron = 1; buzzerpoweron < 9; buzzerpoweron++)
                        {
                            buzzerPlaySong(buzzerFd, buzzerEnableFd, buzzerpoweron); //on상태인지 확인?
                        }
                    }
                    break;

                case 139: //led on/off
                    textlcdmode(1, "LED_ON/OFF");
                    if (ledtoggle == 1) //led,colorled on->off
                    {
                        textlcdmode(2, "  OFF   ");
                        ledtoggle = 0;
                        pwmStartAll();
                    }
                    else //led,colorled off->on
                    {
                        textlcdmode(2, "  ON    ");
                        ledtoggle = 1;

                        pwmStopAll();
                        pwmSetPercentRGB(0, 0);
                        pwmSetPercentRGB(0, 1);
                        pwmSetPercentRGB(0, 2);
                    }
                    break;

                case 115: //fnd on/off
                    textlcdmode(1, "FND_ON/OFF");

                    if (fndtoggle == 1)
                    {
                        textlcdmode(2, "  OFF   ");
                        fndtoggle = 0;
                        //fndonoff=0;
                    }
                    else
                    {
                        textlcdmode(2, "  ON    ");
                        fndtoggle = 1;
                        //fndonoff=1;
                    }
                    break;

                default: //114값 해당, textlcd on/off

                    if (textlcdtoggle == 1)
                    {
                        textlcdtoggle = 0;
                    }

                    else
                    {
                        textlcdtoggle = 1;
                    }
                    break;
            }
        }

        else {
            switch (rcvMsg.keyInput)
            {
                case 999:
                    if (rcvMsg.pressed == 1)
                    {
                        if (screenType == MAIN)
                        {
                            tick_on = 0;
                            if (rcvMsg.x <= 952 && rcvMsg.x > 584 && rcvMsg.y <= 410 && rcvMsg.y > 360)
                            {
                                screenType = BOARD;
                                fb_clear();
                                fb_write(board_data, board_cols, board_rows);
                            }

                            if (rcvMsg.x <= 952 && rcvMsg.x > 584 && rcvMsg.y <= 175 && rcvMsg.y > 135)
                            {
                                screenType = EXIT;
                                fb_clear();
                            }
                        }

                        else if (screenType == BOARD)
                        {
                            if (rcvMsg.x <= 170 && rcvMsg.x > 12 && rcvMsg.y <= 515 && rcvMsg.y > 452)
                            {
                                screenType = MAIN;
                                fb_clear();
                                fb_write(op_data, op_cols, op_rows);
                            }

                            else
                            {
                                timCount = timercount;
                                tick_on = 1;
                            }
                        }
                    }
                    break;
            }
        }

        
    }   

    //if(rxMsg.keyInput == 114 && rxMsg.pressed == 0)
    //{
    //    pthread_exit(NULL);
    //}
    //각 디바이스 종료

    close_bmp();
    fb_close();

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