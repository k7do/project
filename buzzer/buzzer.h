#ifndef _BUZZER_H_
#define _BUZZER_H_
int buzzerInit();
int buzzerPlaySong(int scale);
int buzzerStopSong(void);
int buzzerExit(void);
void buzzerEnable(int bEnable);
void setFrequency(int frequency);
void doHelp(void);
#endif