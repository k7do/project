#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "accelMagGyro.h"

#define ACCELPATH "/sys/class/misc/FreescaleAccelerometer/"
#define MAGNEPATH "/sys/class/misc/FreescaleMagnetometer/"
#define GYROPATH "/sys/class/misc/FreescaleGyroscope/"

int fd = 0;
FILE *fp = NULL;

void accelRead(int* accel)
{
    fd = open (ACCELPATH "enable", O_WRONLY);
    dprintf(fd, "1");
    close(fd);
    fp = fopen(ACCELPATH "data", "rt");

    fscanf(fp, "%d, %d, %d", &accel[0], &accel[1], &accel[2]);
    close(fp);
}

void magRead(int* magne)
{
    fd = open (MAGNEPATH "enable", O_WRONLY);
    dprintf(fd, "1");
    close(fd);
    fp = fopen(MAGNEPATH "data", "rt");

    fscanf(fp, "%d, %d, %d", &magne[0], &magne[1], &magne[2]);
    close(fp);
}

void gyroRead(int* gyro)
{
    fd = open (GYROPATH "enable", O_WRONLY);
    dprintf(fd, "1");
    close(fd);
    fp = fopen(GYROPATH "data", "rt");

    fscanf(fp, "%d, %d, %d", &gyro[0], &gyro[1], &gyro[2]);
    close(fp);
}