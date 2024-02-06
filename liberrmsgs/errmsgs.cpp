#include "errmsgs.h"

const char* err_msgs[] = {
	#define ERR(C) #C,
	#include "err.def"
};
