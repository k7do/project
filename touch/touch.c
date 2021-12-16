#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>    // for open/close
#include <fcntl.h>     // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/msg.h>
#include <pthread.h>
#include "touch.h"
// first read input device
#define INPUT_DEVICE_LIST "/dev/input/event"
#define PROBE_FILE "/proc/bus/input/devices"
#define HAVE_TO_FIND_1 "N: Name=\"WaveShare WaveShare Touchscreen\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=mouse0 event"
#define MUTEX_ENABLE 0

int readSize;
static int fd = 0;
static struct input_event stEvent;
int T_threadIndex = 0;
TOUCH_MSG_T txMsg;
char T_inputDevPath[200] = {
    0,
};
int T_msgID;
pthread_mutex_t lock;

int probetouchPath(char *newPath)
{
    int returnValue = 0;                //touch에 해당하는 event#을 찾았나?
    int number = 0;                     //찾았다면 여기에 집어넣자
    FILE *fp = fopen(PROBE_FILE, "rt"); //파일을 열고

    while (!feof(fp)) //끝까지 읽어들인다.
    {
        char tmpStr[200];       //200자를 읽을 수 있게 버퍼
        fgets(tmpStr, 200, fp); //최대 200자를 읽어봄
        //printf ("%s",tmpStr);
        if (strcmp(tmpStr, HAVE_TO_FIND_1) == 0)
        {
            printf("YES! I found!: %s\r\n", tmpStr);
            returnValue = 1; //찾음
        }

        if ((returnValue == 1) && (strncasecmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0))
        {
            printf("-->%s", tmpStr);
            printf("\t%c\r\n", tmpStr[strlen(tmpStr) - 3]);
            number = tmpStr[strlen(tmpStr) - 3] - '0';
            //Ascii character '0'-'9' (0x30-0x39)
            //to interger(0)
            break;
        }
    }
    fclose(fp);

    if (returnValue == 1)
        sprintf(newPath, "%s%d", INPUT_DEVICE_LIST, number);

    return returnValue;
}

void *touchThFunc(void *arg)
{
#if MUTEX_ENABLE               // mutex을 사용했을 경우
    pthread_mutex_lock(&lock); // è lock으로 다른 스레드의 동시 수행 차단
#endif

    int x = 0;
    int y = 0;

    txMsg.messageNum = 1;
    txMsg.keyInput = 999;
    while (1)
    {
        readSize = read(fd, &stEvent, sizeof(stEvent));

        if(stEvent.type == EV_ABS)
        {
            if(stEvent.code == ABS_MT_POSITION_X)
            {
                x = stEvent.value;
                //printf( "touch X: %d\r\n", stEvent.value);
            }

            if(stEvent.code == ABS_MT_POSITION_Y)
            {
                y = stEvent.value;
                //printf( "touch Y: %d\r\n", stEvent.value);
            }
        }

        else if ((stEvent.type == EV_KEY ) && (stEvent.code == BTN_TOUCH) )
        {
            txMsg.messageNum = 200;
            txMsg.x = x;
            txMsg.y = y;
            if(stEvent.value == 0)
            {
                txMsg.pressed = 0;
            }

            else if(stEvent.value == 1)
            {
                txMsg.pressed = 1;
            }

            msgsnd(T_msgID, &txMsg, sizeof(TOUCH_MSG_T) - sizeof(long int), 0);
        }
        /*
        if (readSize != sizeof(stEvent))
        {
            continue;
        }

        if (stEvent.type == EV_KEY)
        {
            txMsg.messageNum++;
            txMsg.keyInput = stEvent.code;
            txMsg.pressed = stEvent.value;

            msgsnd(T_msgID, &txMsg, sizeof(touch_MSG_T), 0);
        }
        //msgsnd(msgQueue, &messageTxData, sizeof(messageTxData.piggyBack), 0);
        */
    }

#if MUTEX_ENABLE
    pthread_mutex_unlock(&lock); // 다른 스레드가 수행할수 있도록 lock 해제
#endif

    return NULL;
}

void touchInit(pthread_t *touchTh_id) // 작성 할 것
{
    if (probetouchPath(T_inputDevPath) == 0)
    {
        printf("ERROR! File Not Found!\r\n");
        printf("Did you insmod?\r\n");
    }

    printf("T_inputDevPath: %s\r\n", T_inputDevPath);

    txMsg.messageNum = 0;

    fd = open(T_inputDevPath, O_RDONLY);
    if (fd == -1)
        printf("file open error\r\n");

    T_msgID = msgget(T_MESSAGE_ID, IPC_CREAT | 0666);

    if (T_msgID == -1)
    {
        printf("Cannot get msgQueueID, Return!\r\n");
    }

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex Init Failed !!\n");
    }

    pthread_create(touchTh_id, NULL, &touchThFunc, NULL);

}

int touchExit(void)
{
    close(fd);
}

