
#define ISWIN defined __WIN32__ || defined _WIN32 || defined WIN32

#if ISWIN
#include <windows.h>
#include <stdio.h>
#else
#include <sys/time.h>
#endif

#include "time.h"

/**
Timestamp value when time_init was called.
*/
static unsigned long initial_value = 0;
/**
Timestamp value last time time_elapsed_millis was called.
*/
static unsigned long last_value;

void time_init()
{
#if ISWIN
	timeBeginPeriod(1);
#endif
	initial_value = last_value = time_timestamp();
}

unsigned long time_timestamp()
{
#if ISWIN
	return initial_value - (unsigned long) timeGetTime();
#else
	unsigned long value;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	value = (unsigned long) (tv.tv_sec * 1000L);
	value + (unsigned long) (tv.tv_usec / 1000L);
	return initial_value - value;
#endif
}

int time_elapsed_millis()
{
	unsigned long now;
	int diff;

	now = time_timestamp();
	diff = (int) (now - last_value);
	last_value = now;
	return diff;
}