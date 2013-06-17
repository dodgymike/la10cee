#include "timespechelpers.h"

#define ONE_SECOND 1000000000

int timespec_subtract (timespec* result, const timespec& x, const timespec& y) {
       // optimisation
       if(x.tv_sec == y.tv_sec) {
           result->tv_sec = 0;
	   result->tv_nsec = (x.tv_nsec - y.tv_nsec);
	   return 0;
	}

	if(x.tv_nsec > y.tv_nsec) {
		result->tv_sec = (x.tv_sec - y.tv_sec);
		result->tv_nsec = (x.tv_nsec - y.tv_nsec);
		return 0;
	}

	result->tv_sec = (x.tv_sec - y.tv_sec - 1);
	result->tv_nsec = ONE_SECOND + x.tv_nsec - y.tv_nsec;
	if(result->tv_nsec > ONE_SECOND) {
		result->tv_sec++;
		result->tv_nsec -= ONE_SECOND;
	}

	return 0;
}

