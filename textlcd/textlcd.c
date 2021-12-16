#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LINE_NUM			2
#define COLUMN_NUM			16	


#define  LINE_BUFF_NUM 	(COLUMN_NUM + 4)  // for dummy 

#define  MODE_CMD	0
#define  MODE_DATA	1

#define  CMD_DISPLAY_MODE		0x10
	#define  BIT_DISPLAY_MODE_CURSOR_DISP	0x01
	#define  BIT_DISPLAY_MODE_CURSOR_BLINK	0x02
	#define  BIT_DISPLAY_MODE_DISP_ENABLE	0x04

#define  CMD_CURSOR_MOVE_MODE	0x11
	#define CURSOR_MOVE_MODE_ENABLE		0x01
	#define CURSOR_MOVE_MODE_RIGHT_DIR	0x02

#define  CMD_CURSOR_MOVE_POSITION	0x12
	#define  CURSOR_MOVE_X_MASK		0x3F
	#define  CURSOR_MOVE_Y_MASK		0xC0
	
#define  CMD_WRITE_STRING			0x20	
	#define CMD_DATA_WRITE_BOTH_LINE	0
	#define CMD_DATA_WRITE_LINE_1		1
	#define CMD_DATA_WRITE_LINE_2		2
	
#define  CMD_TEST_GPIO_HIGH			0x30
#define  CMD_TEST_GPIO_LOW			0x31
#define TEXTLCD_DRIVER_NAME		"/dev/peritextlcd"

#include "textlcd.h"
static int fd;
static int len; 
stTextLCD  stlcd; 

/*
void doHelp(void)
{
	printf("usage: textlcdtest <linenum> <'string'>\n");
	printf("       linenum => 1 ~ 2\n");	
	printf("  ex) textlcdtest 2 'test hello'\n");
	
}*/

int textlcdinit(void)
{
// open  driver 
	fd = open(TEXTLCD_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver (//dev//peritextlcd) open error.\n");
		return 1;
	}
    return 0;
}
void textlcdexit()
{
	close(fd);
}

//int textlcdmode(int argc , char **argv)
int textlcdmode(unsigned int linenumber , char string[])
{
	//unsigned int linenum = 0;
	
	memset(&stlcd,0,sizeof(stTextLCD));
	/*
	if (argc < 3 )
	{
		perror(" Args number is less than 2\n");
		doHelp();
		return 1;
	}*/
	
	//작성할 라인 넘버 입력linenum = strtol(argv[1],NULL,10);
	//printf("linenum :%d\n", linenum);
	
	if ( linenumber == 1) // firsst line
	{
		stlcd.cmdData = CMD_DATA_WRITE_LINE_1;
	}
	else if ( linenumber == 2) // second line
	{
		stlcd.cmdData = CMD_DATA_WRITE_LINE_2;
	}
	else 
	{
		printf("linenumber : %d  wrong .  range (1 ~ 2)\n", linenumber);
		return 1; 
	}
	printf("string:%s\n",string);
	len = strlen(string);
	if ( len > COLUMN_NUM)
	{
		memcpy(stlcd.TextData[stlcd.cmdData - 1],string,COLUMN_NUM);
	}
	else
	{
		memcpy(stlcd.TextData[stlcd.cmdData - 1],string,len);
	}
	stlcd.cmd = CMD_WRITE_STRING;
    write(fd,&stlcd,sizeof(stTextLCD));
	return 0;
}
