#ifndef _LM74_H_
#define _LM74_H_

int spi_init(char fileName[40]);
double spi_read_lm74(void);

#endif