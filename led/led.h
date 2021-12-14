#ifndef _LED_H_
#define _LED_H_
int ledOn(int fd, int ledNum);
int ledInit(void);
int ledExit(int fd);
#define LED_DRIVER_NAME "/dev/periled"
#endif _LED_H_
