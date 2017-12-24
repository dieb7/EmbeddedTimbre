// Generic stats counter for a file  Robert Chapman  Dec 15, 2017

// 14 MHz clock ticks; wraps after 5 minutes; 71.4 ns resolution
#ifndef CLOCK_MHZ
  #define CLOCK_MHZ 14
#endif

#define getTicks() (*(Long *)(0xE0001004)) // cortex-M3
#define CONVERT_TO_US(n) ((n)/CLOCK_MHZ)
#define CONVERT_TO_MS(n) ((n)/(CLOCK_MHZ*1000))

#ifdef FOR_EACH_STAT
/*
 use to increment stat counters within code to find out which paths are being used
 at run time.
 To use, define a list like this:
    #define FOR_EACH_STAT(F) \
        F(BadSync) \
        F(GoodFrame) \
        F(BadCheckSum) \
        F(RxTimeout) \
        F(BadLength) \
 and then include this file

 To see your stats invoke the function printStats() which will also clear the stats.
 Only non zero stats are printed.
 */

#include "printers.h"
#include <string.h>

#define ENUM_STAT(stat) stat,

enum{FOR_EACH_STAT(ENUM_STAT) NUM_STATS};

static Cell statCounts[NUM_STATS] = {0};

#define PRINT_STAT(stat) \
	if (statCounts[stat]) \
		print("\n"#stat": "), printDec(statCounts[stat]);

static void printStats() {
    FOR_EACH_STAT(PRINT_STAT)
    memset(statCounts, 0, sizeof(statCounts));
}

// use these macros to use the stats
#define getStat(stat) statCounts[stat]
#define setStat(stat, val) statCounts[stat] = val
#define incStat(stat) statCounts[stat]++
#define maxStat(n, stat) if ((n) > statCounts[stat]) statCounts[stat] = n

// Time measures in microseconds
#define startUS() Cell startingTime = getTicks()
#define maxUS(stat) maxStat(CONVERT_TO_US(getTicks() - startingTime), stat)

// Time measures in milliseconds
#define startMS() startUS()
#define maxMS(stat) maxStat(CONVERT_TO_MS(getTicks() - startingTime), stat)

#else
typedef int make_iso_compilers_happy;
#endif
