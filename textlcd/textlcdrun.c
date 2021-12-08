#include <stdio.h>
#include "textlcd.h"

int main(int argc , char **argv)
{
    textlcdinit();
    textlcdmode(argc, argv);
    //textlcdexit();
	return 0;
}