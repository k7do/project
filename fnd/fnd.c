#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "fnd.h"

#define FND_DRIVER_NAME		"/dev/perifnd"
#define MODE_STATIC_DIS		0
#define MODE_TIME_DIS		1
#define MODE_COUNT_DIS		2

static	int mode ;
static	int number,counter;
/*
void doHelp(void)
{
	printf("Usage:\n");
	printf("fndtest [option] [displaynum] [maxcounter]  >\n");
	printf("option   s  : static display  , displaynum range 0 ~ 999999\n");
	printf("option   t  ; time display  \n"); 
	printf("option   c  : count from 0 to maxcounter .\n");
	printf("option   o  : fnd off\n");
	printf("ex) fndtest s '123456'  ; display  '123456' \n");
	printf("ex) fndtest t 		;display current time \n");
	printf("ex) fndtest c 10 ; display counting number  from  0  to  10  with 1 Second interval.\n");
	printf("ex) fndtest 0		;display off \n");
}
*/

 
//int fndmode(int argc, char**argv)
//프로적트에서는
int fndmode(char fndchar, int fndnumber)
{
	/* 우선 인자를 다 정하기때문에 인자 개수 판별은 제외했습니다
	if (argc <  2)
	{
		//doHelp();
		return 1;
	}*/

	if( fndchar == 's') //if ( argv[1][0] == 's'  )
	{
		mode = MODE_STATIC_DIS;
		/*
		if ( argc < 3 )
		{
			perror(" Args number is less than 3\n");
			//doHelp();
			return 1;
		}
		number = atoi(argv[2]);*/
		number = fndnumber;
	}

	
	else if( fndchar == 't')//else if ( argv[1][0] == 't'  )
	{
		mode = MODE_TIME_DIS;
		/*
		if ( argc < 2 )
		{
			perror(" Args number is less than 2\n");
			//doHelp();
			return 1;
		}*/
	}

	else if( fndchar == 'c')	//else if ( argv[1][0] == 'c'  )
	{
		mode = MODE_COUNT_DIS;
		/*if ( argc < 3 )
		{
			perror(" Args number is less than 3\n");
			//doHelp();
			return 1;
		}
		number = atoi(argv[2]);*/
		number = fndnumber;
	}

	else if(fndchar == 'o')//else if (argv[1][0] == 'o' )
	{
		fndOff();
		return 0; 
	}

	else
	{
		//doHelp();
		perror("option error \n");
		return 1;
	}

	printf("number :%d\n",number);

	// open  driver 
	if (mode == MODE_STATIC_DIS )
	{
		fndDisp(number , 0);
	}

	else if(mode == MODE_TIME_DIS )
	{
		struct tm *ptmcur;
		time_t tTime;
		if ( -1 == time(&tTime) )
			return -1; 
		ptmcur = localtime(&tTime);
		number = ptmcur->tm_hour * 10000;
		number += ptmcur->tm_min *100;
		number += ptmcur->tm_sec;
		fndDisp(number , 0b1010);
	}

	else if (mode == MODE_COUNT_DIS)
	{
		counter = number;
		while(1)
		{
			if (!fndDisp(counter , 0))
				break;
			counter--;
			sleep(1);
			if (counter <= 0 )
				break;
		}
	}
	return 1;
}


#define ONE_SEG_DISPLAY_TIME_USEC	1000
// return 1 => success  , 0 => error
int fndDisp(int num , int dotflag)
{
	int fd;
	int temp,i;
	stFndWriteForm stWriteData;
	
	for (i = 0; i < MAX_FND_NUM ; i++ )
	{
		stWriteData.DataDot[i] = (dotflag & (0x1 << i)) ? 1 : 0;  
		stWriteData.DataValid[i] = 1;
	}
	// if 6 fnd
	temp = num % 1000000;	stWriteData.DataNumeric[0]= temp /100000;
	temp = num % 100000;	stWriteData.DataNumeric[1]= temp /10000;
	temp = num % 10000;		stWriteData.DataNumeric[2] = temp /1000;
	temp = num %1000;		stWriteData.DataNumeric[3] = temp /100;
	temp = num %100;		stWriteData.DataNumeric[4] = temp /10;
							stWriteData.DataNumeric[5] = num %10;

	fd = open(FND_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver open error.\n");
		return 0;
	}	
	write(fd,&stWriteData,sizeof(stFndWriteForm));
	close(fd);
	return 1;
}

int fndOff()
{
	int fd,i;
	stFndWriteForm stWriteData;
	
	for (i = 0; i < MAX_FND_NUM ; i++ )
	{
		stWriteData.DataDot[i] =  0;  
		stWriteData.DataNumeric[i] = 0;
		stWriteData.DataValid[i] = 0;
	}
	fd = open(FND_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver open error.\n");
		return 0;
	}	
	write(fd,&stWriteData,sizeof(stFndWriteForm));
	close(fd);
	return 1;
}