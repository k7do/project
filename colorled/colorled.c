#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "colorled.h"

void doHelp(void)
{
	printf("Usage:Color LED\n");
	printf("colorledrun  red green blue\n");
	printf("color range 0 ~ 100\n");
	printf("ex) colorledrun  100 100 100 =>white color, all full on\n");
}

int pwmActiveAll(void)
{
    int fd = 0;
    fd = open ( COLOR_LED_DEV_R_ PWM_EXPORT, O_WRONLY);
    write(fd,&"0",1);
    close(fd);
    fd = open ( COLOR_LED_DEV_G_ PWM_EXPORT, O_WRONLY);
    write(fd,&"0",1);
    close(fd);
    fd = open ( COLOR_LED_DEV_B_ PWM_EXPORT, O_WRONLY);
    write(fd,&"0",1);
    close(fd);
    return 1;
}

int pwmInactiveAll(void)
{
    int fd = 0;
    fd = open ( COLOR_LED_DEV_R_ PWM_UNEXPORT, O_WRONLY);
    write(fd,&"0",1);
    close(fd);
    fd = open ( COLOR_LED_DEV_G_ PWM_UNEXPORT, O_WRONLY);
    write(fd,&"0",1);
    close(fd);
    fd = open ( COLOR_LED_DEV_B_ PWM_UNEXPORT, O_WRONLY);
    write(fd,&"0",1);
    close(fd);
    return 1;
}

int pwmSetDuty(int dutyCycle, int pwmIndex)
{
    int fd = 0;
    switch (pwmIndex)
    {
        case 2:
            fd = open ( COLOR_LED_DEV_B_ PWM_DUTY, O_WRONLY);
            break;
        case 1:
            fd = open ( COLOR_LED_DEV_G_ PWM_DUTY, O_WRONLY);
            break;
        case 0:
            default:
            fd = open ( COLOR_LED_DEV_R_ PWM_DUTY, O_WRONLY);
        break;
    }
    dprintf(fd, "%d", dutyCycle);
    close(fd);
    return 1;
}

int pwmSetPeriod(int Period, int pwmIndex)
{
    int fd = 0;
    switch (pwmIndex)
    {
        case 2:
            fd = open ( COLOR_LED_DEV_B_ PWM_PERIOD, O_WRONLY);
            break;
        case 1:
            fd = open ( COLOR_LED_DEV_G_ PWM_PERIOD, O_WRONLY);
            break;
        case 0:
        default:
            fd = open ( COLOR_LED_DEV_R_ PWM_PERIOD, O_WRONLY);
        break;
    }
    dprintf(fd, "%d", Period);
    close(fd);
    return 1;
}

int pwmSetPercentRGB(int percent, int ledColor)
{
    if ((percent <0) || (percent > 100))
    {
        printf ("Wrong percent: %d\r\n",percent);
        return 0;
    }
    int duty = (100- percent) * PWM_PERIOD_NS / 100;
    //LED Sinking.
    pwmSetDuty(duty, ledColor);
    return 0;
}

int pwmSetWave(int percent)
{
        pwmSetPercentRGB(0,0);
        pwmSetPercentRGB(0,1);
        pwmSetPercentRGB(0,2);
    if ((percent <0) || (percent > 100))
    {
        printf ("Wrong percent: %d\r\n",percent);
        return 0;
    }
    int duty,i,j,k,fd = 0;
    for(i=0;i<3;i++)
    {
        switch (i)
        {
        case 2:
            fd = open ( COLOR_LED_DEV_B_ PWM_DUTY, O_WRONLY);
            break;
        case 1:
            fd = open ( COLOR_LED_DEV_G_ PWM_DUTY, O_WRONLY);
            break;
        case 0:
            default:
            fd = open ( COLOR_LED_DEV_R_ PWM_DUTY, O_WRONLY);
        break;
        }
        for(j=0;j<=percent;j++)
        {
            duty = (100- j) * PWM_PERIOD_NS / 100;
            dprintf(fd, "%d", duty);
            for(k=0;k<0xFFFFF;k++){}
        }
        for(j=percent;j>=0;j--)
        {
            duty = (100- j) * PWM_PERIOD_NS / 100;
            dprintf(fd, "%d", duty);
            for(k=0;k<0xFFFFF;k++){}
        }
    }
    close(fd);
    return 1;
}

int pwmStartAll(void)
{
    int fd = 0;
    fd = open ( COLOR_LED_DEV_R_ PWM_ENABLE, O_WRONLY);
    write(fd,&"1",1);
    close(fd);
    fd = open ( COLOR_LED_DEV_G_ PWM_ENABLE, O_WRONLY);
    write(fd,&"1",1);
    close(fd);
    fd = open ( COLOR_LED_DEV_B_ PWM_ENABLE, O_WRONLY);
    write(fd,&"1",1);
    close(fd);
    return 1;
}

int pwmLedInit(void)
{   //Initialize
    pwmActiveAll();
    pwmSetDuty(0, 0); //R<-0
    pwmSetDuty(0, 1); //G<-0
    pwmSetDuty(0, 2); //B<-0
    pwmSetPeriod(PWM_PERIOD_NS, 0);
    pwmSetPeriod(PWM_PERIOD_NS, 1);
    pwmSetPeriod(PWM_PERIOD_NS, 2);
    pwmStartAll();
    return 0;
}