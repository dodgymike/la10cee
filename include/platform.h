#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>

// With help from https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
#ifdef __linux
        #include <netinet/ether.h>
	#include <asm-generic/errno-base.h>
#elif __MACH__
	#include <net/ethernet.h>
	#include <sys/errno.h>
	#include <mach/clock.h>
	#include <mach/mach.h>
#else
        #error "Unsupported platform, please help us out by contributing a fix!"
#endif

#endif
