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
#define B_INPUT_DEVICE_LIST "/dev/input/event"
#define B_PROBE_FILE "/proc/bus/input/devices"
#define B_HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define B_HAVE_TO_FIND_2 "H: Handlers=kbd event"
#define MUTEX_ENABLE 0

int B_readSize;
static int B_fd = 0;
static struct input_event B_stEvent;
int B_threadIndex = 0;
BUTTON_MSG_T B_txMsg;
char B_inputDevPath[200] = {
    0,
};
int B_msgID;
pthread_mutex_t lock;

void *buttonThFunc(void *arg)
{

    while (1)
    {
        B_readSize = read(B_fd, &B_stEvent, sizeof(B_stEvent));
        if (B_readSize != sizeof(B_stEvent))
        {
            continue;
        }

        if (B_stEvent.type == EV_KEY)
        {
            B_txMsg.MessageNum = 199;
            B_txMsg.keyInput = B_stEvent.code;
            B_txMsg.pressed = B_stEvent.value;
            msgsnd(B_msgID, &B_txMsg, sizeof(BUTTON_MSG_T) - sizeof(long int), 0);
        }
        //msgsnd(msgQueue, &messageTxData, sizeof(messageTxData.piggyBack), 0);
    }
    

    return NULL;
}

int buttonInit(pthread_t* buttonTh_id) // 작성 할 것
{

    if (probeButtonPath(B_inputDevPath) == 0)
    {
        printf("ERROR! File Not Found!\r\n");
        printf("Did you insmod?\r\n");
        return 0;
    }

    printf("B_inputDevPath: %s\r\n", B_inputDevPath);

    B_fd = open(B_inputDevPath, O_RDONLY);
    if (B_fd == -1)
        printf("file open error\r\n");

    B_msgID = msgget(B_MESSAGE_ID, IPC_CREAT | 0666);

    if (B_msgID == -1)
    {
        printf("Cannot get msgQueueID, Return!\r\n");
        return -1;
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
    FILE *fp = fopen(B_PROBE_FILE, "rt"); //파일을 열고

    while (!feof(fp)) //끝까지 읽어들인다.
    {
        char tmpStr[200];       //200자를 읽을 수 있게 버퍼
        fgets(tmpStr, 200, fp); //최대 200자를 읽어봄
        //printf ("%s",tmpStr);
        if (strcmp(tmpStr, B_HAVE_TO_FIND_1) == 0)
        {
            printf("YES! I found!: %s\r\n", tmpStr);
            returnValue = 1; //찾음
        }

        if ((returnValue == 1) && (strncasecmp(tmpStr, B_HAVE_TO_FIND_2, strlen(B_HAVE_TO_FIND_2)) == 0))
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
        sprintf(newPath, "%s%d", B_INPUT_DEVICE_LIST, number);

    return returnValue;
}