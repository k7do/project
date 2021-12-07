#include <stdio.h>

#include <string.h>
#include "colorled.h"


int main(int argc, char *argv[])
{
        pwmLedInit();
        if(!strcmp("w",argv[1]))
        {
            pwmSetWave(atoi(argv[2]));
        }
        else
        {
            if (argc != 4)
            {
                printf ("colorledtest.elf 0-100 0-100 0-100\r\n");
                printf ("ex) colorledtest.elf 100 100 100 ==> full white color\r\n");
                return 0;
            }
        pwmSetPercentRGB(atoi(argv[1]),0);
        pwmSetPercentRGB(atoi(argv[2]),1);
        pwmSetPercentRGB(atoi(argv[3]),2);
        }
        
        
        //pwmInactiveAll();
    return 0;
}