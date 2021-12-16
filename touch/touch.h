#ifndef _TOUCH_H_
#define _TOUCH_H_
#define T_MESSAGE_ID 1123

typedef struct
{
    long int messageNum;
    int keyInput;
    int pressed;
    int x;
    int y;
} TOUCH_MSG_T;

int probetouchPath(char *newPath);
void *touchThFunc(void *arg);
void touchInit(pthread_t *touchTh_id);
int touchExit(void);

#endif