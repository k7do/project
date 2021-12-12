#ifndef _TOUCH_H_
#define _TOUCH_H_
#define MESSAGE_ID 1123

typedef struct
{
    long int messageNum;
    int keyInput;
    int pressed;
    int x;
    int y;
} TOUCH_MSG_T;

void touchInit(void);
int touchExit(void);

#endif