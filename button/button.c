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
int fd=0;
struct input_event stEvent;
int threadIndex = 0;
BUTTON_MSG_T txMsg;
char inputDevPath[200] = {0, };
int msgID;
pthread_t buttonTh_id;
pthread_mutex_t lock;

void *buttonThFunc(void *arg)
{
    #if MUTEX_ENABLE               // mutex을 사용했을 경우
        pthread_mutex_lock(&lock); // è lock으로 다른 스레드의 동시 수행 차단
    #endif

    while(1)
    {
        readSize = read(fd, &stEvent, sizeof(stEvent));
        if (readSize != sizeof(stEvent))
        {
            continue;
        }
        
        if (stEvent.type == EV_KEY)
        {
            txMsg.messageNum++;
            txMsg.keyInput = stEvent.code;
            txMsg.pressed = stEvent.value;

            msgsnd(msgID, &txMsg, sizeof(BUTTON_MSG_T), 0);
        }
        //msgsnd(msgQueue, &messageTxData, sizeof(messageTxData.piggyBack), 0);
    }

    #if MUTEX_ENABLE
        pthread_mutex_unlock(&lock); // 다른 스레드가 수행할수 있도록 lock 해제
    #endif

        return NULL;
}
