#ifndef _BUZZER_H_
#define _BUZZER_H_

#define MAX_SCALE_STEP 8
#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices/"
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"

int buzzerInit(int *enableFd);
int buzzerPlaySong(int fd, int enableFd, int scale);
void buzzerExit(int enableFd, int fd);
void doHelp(void);

#endif