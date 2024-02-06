#include "errmsgs.h"

char* err_msgs[] = {
	#define ERR(C) #C,
	#include "err.def"
};
