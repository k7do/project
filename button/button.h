#ifndef _BUTTON_H_
#define _BUTTON_H_
#define B_MESSAGE_ID 1122

typedef struct
{
    long int MessageNum;
    int keyInput;
    int pressed;
} BUTTON_MSG_T;

void *buttonThFunc(void *arg);
int buttonInit(pthread_t* buttonTh_id;);
int buttonExit(void);
int probeButtonPath(char *newPath);

#define INPUT_DEVICE_LIST "/dev/input/event" //실제 디바이스 드라이버 노드파일: 뒤에 숫자가 붙음., ex)/dev/input/event5
#define PROBE_FILE "/proc/bus/input/devices" //PPT에 제시된 "이 파일을 까보면 event? 의 숫자를 알수 있다"는 바로 그 파일
#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"
#define MUTEX_ENABLE 0

#endif