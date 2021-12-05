#ifndef _LED_H_
#define _LED_H_
int ledLibInit(void);
int ledOn(int ledNum);
int ledLibExit(void);
#define LED_DRIVER_NAME "/dev/periled"
#endif _LED_H_