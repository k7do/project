#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"

int main(int argc , char **argv)
{
    int freIndex;

    if (argc < 2 || buzzerInit() )
    {
        printf("Error!\n");
