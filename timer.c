
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "timer.h"
#include "time/time.h"
#include <string.h>

/*arbitrary value*/
#define MAX_TIMERS (500)

struct TIMER {
	int initial_time;
	int time_left;
	int repeating;
	timer_cb callback;
	void *data;
};

static struct TIMER timers[MAX_TIMERS];
static int numtimers;

void timer_reset()
{
	time_elapsed_millis();
}

void timer_set(int interval, int repeating, timer_cb cb, void *data)
{
	struct TIMER *timer;

#if DEV
	if (!time_is_inited()) {
		logprintf("ERR: don't call timer_set before time is inited!");
		return;
	}
#endif

	if (numtimers >= MAX_TIMERS - 1) {
		logprintf("ERR: timer limit "Q(MAX_TIMERS)" reached");
		return;
	}

	timer = timers + numtimers;
	timer->initial_time = interval;
	timer->time_left = interval;
	timer->repeating = repeating;
	timer->callback = cb;
	timer->data = data;
	numtimers++;
}

void timer_tick(AMX *amx)
{
	int i, elapsed_time;

	elapsed_time = time_elapsed_millis();
	for (i = 0; i < numtimers; i++) {
		timers[i].time_left -= elapsed_time;
		if (timers[i].time_left <= 0) {
			timers[i].callback(amx, timers[i].data);
			if (timers[i].repeating) {
				timers[i].time_left += timers[i].initial_time;
			} else {
				if (i != numtimers - 1) {
					memcpy(timers + i,
						timers + numtimers - 1,
						sizeof(struct TIMER));
					i--;
				}
				numtimers--;
			}
		}
	}
}