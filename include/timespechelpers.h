#ifndef _TIMESPEC_HELPERS_H 
#define _TIMESPEC_HELPERS_H

#include <time.h>

     /* Subtract the `struct timespec' values X and Y,
 *         storing the result in RESULT.
 *                 Return 1 if the difference is negative, otherwise 0. */
     
     int timespec_subtract (timespec* result, const timespec& x, const timespec& y);
#endif
