
#define ISWIN defined __WIN32__ || defined _WIN32 || defined WIN32

#if ISWIN
#include <windows.h>
#include <stdio.h>
#else
#include <unistd.h>
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
	if (!initial_value) {
#if ISWIN
		timeBeginPeriod(1);
#endif
		initial_value = time_timestamp();
		last_value = 0;
	}
}

int time_is_inited()
{
	return initial_value;
}

unsigned long time_timestamp()
{
#if ISWIN
	return (unsigned long) timeGetTime() - initial_value;
#else
	unsigned long value;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	value = (unsigned long) (tv.tv_sec * 1000L);
	value + (unsigned long) (tv.tv_usec / 1000L);
	return value - initial_value;
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

void time_sleep(int millis)
{
#if ISWIN
	Sleep(millis);
#else
	usleep(millis * 1000);
#endif
}