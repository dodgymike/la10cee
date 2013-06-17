#include "timespechelpers.h"

int timespec_subtract (timespec* result, const timespec& x, const timespec& y) {
       timespec workingCopy;
       
       workingCopy.tv_sec = y.tv_sec;
       workingCopy.tv_nsec = y.tv_nsec;

       /* Perform the carry for the later subtraction by updating y. */
       if (x.tv_nsec < workingCopy.tv_nsec) {
         int nsec = (workingCopy.tv_nsec - x.tv_nsec) / 1000000 + 1;
         workingCopy.tv_nsec -= 1000000 * nsec;
         workingCopy.tv_sec += nsec;
       }
       if (x.tv_nsec - workingCopy.tv_nsec > 1000000) {
         int nsec = (x.tv_nsec - workingCopy.tv_nsec) / 1000000;
         workingCopy.tv_nsec += 1000000 * nsec;
         workingCopy.tv_sec -= nsec;
       }

       /* Compute the time remaining to wait.
 *  *           tv_nsec is certainly positive. */
       result->tv_sec = x.tv_sec - workingCopy.tv_sec;
       result->tv_nsec = x.tv_nsec - workingCopy.tv_nsec;

       /* Return 1 if result is negative. */
       return x.tv_sec < workingCopy.tv_sec;
}

