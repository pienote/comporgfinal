#include "pipeline.h"
#include <string.h>

int main(int argc, char* argv[])
{
	bool forward;
	if(argc != 3)
		return -1;
	if(strcmp(argv[1], "F") == 0)
		forward = true;
	else if(strcmp(argv[1], "N") == 0)
		forward = false;
	else
		return -1;
	pipeline pl(forward, argv[2]);
	pl.init();
	return 0;
}