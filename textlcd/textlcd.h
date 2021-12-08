#ifndef __TEXTLCD_DRV_H__
#define __TEXTLCD_DRV_H__

int textlcdinit(void);
void textlcdexit();
int textlcdmode(int argc , char **argv);

typedef struct TextLCD_tag 
{
	unsigned char cmd; 
	unsigned char cmdData;  
	unsigned char reserved[2];
	char	TextData[LINE_NUM][LINE_BUFF_NUM];
}stTextLCD,*pStTextLCD;

#endif//  __TEXTLCD_DRV_H__
