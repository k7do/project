#ifndef __FND_DRV_H__
#define __FND_DRV_H__

void *fndThFunc(void *arg);
int fndDisp(int num , int dotflag);
int fndOff();
int fndmode(char fndchar, int fndnumber);

#define MAX_FND_NUM 6
#define FND_DATA_BUFF_LEN (MAX_FND_NUM + 2)
typedef struct FNDWriteDataForm_tag
{
    char DataNumeric[FND_DATA_BUFF_LEN]; //숫자 0-9
    char DataDot[FND_DATA_BUFF_LEN]; //숫자 0 or 1
    char DataValid[FND_DATA_BUFF_LEN]; //숫자 0 or 1
}stFndWriteForm,*pStFndWriteForm;

#endif// __FND_DRV_H__
