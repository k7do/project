#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>    // for open/close
#include <fcntl.h>     // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/msg.h>
#include <pthread.h>
#include "button.h"
// first read input device
#define INPUT_DEVICE_LIST "/dev/input/event"
#define PROBE_FILE "/proc/bus/input/devices"
#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"
#define MUTEX_ENABLE 0

int readSize;
static int fd = 0;
static struct input_event stEvent;
int threadIndex = 0;
BUTTON_MSG_T txMsg;
char inputDevPath[200] = {
    0,
};
int msgID;
pthread_mutex_t lock;

void *buttonThFunc(void *arg)
{
#if MUTEX_ENABLE               // mutex을 사용했을 경우
    pthread_mutex_lock(&lock); // è lock으로 다른 스레드의 동시 수행 차단
#endif

    while (1)
    {
        readSize = read(fd, &stEvent, sizeof(stEvent));
        if (readSize != sizeof(stEvent))
        {
            continue;
        }

        if (stEvent.type == EV_KEY)
        {
            txMsg.keyInput = stEvent.code;
            txMsg.pressed = stEvent.value;
            msgsnd(msgID, &txMsg, sizeof(BUTTON_MSG_T), 0);
            if(txMsg.keyInput == 114 && txMsg.pressed == 0)
            {
                break;
            }
        }
        //msgsnd(msgQueue, &messageTxData, sizeof(messageTxData.piggyBack), 0);
    }

#if MUTEX_ENABLE
    pthread_mutex_unlock(&lock); // 다른 스레드가 수행할수 있도록 lock 해제
#endif

    return NULL;
}

int buttonInit(pthread_t* buttonTh_id) // 작성 할 것
{

    if (probeButtonPath(inputDevPath) == 0)
    {
        printf("ERROR! File Not Found!\r\n");
        printf("Did you insmod?\r\n");
        return 0;
    }

    printf("inputDevPath: %s\r\n", inputDevPath);

    fd = open(inputDevPath, O_RDONLY);
    if (fd == -1)
        printf("file open error\r\n");

    msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);

    if (msgID == -1)
    {
        printf("Cannot get msgQueueID, Return!\r\n");
        return -1;
    }

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex Init Failed !!\n");
        return 1;
    }

    pthread_create(buttonTh_id, NULL, &buttonThFunc, NULL);
    
    return 1;
}

int buttonExit(void)
{
    
}

int probeButtonPath(char *newPath)
{
    int returnValue = 0;                //button에 해당하는 event#을 찾았나?
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