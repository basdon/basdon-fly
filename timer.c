
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "timer.h"
#include "time/time.h"
#include <string.h>

/*arbitrary value*/
#define MAX_TIMERS (500)

struct TIMER {
	int time_left;
	timer_cb callback;
	void *data;
};

static struct TIMER timers[MAX_TIMERS];
static int numtimers;

void timer_reset()
{
	time_elapsed_millis();
}

void timer_set(int interval, timer_cb cb, void *data)
{
	struct TIMER *timer;

#if DEV
	if (!time_is_inited()) {
		logprintf("ERR: don't call timer_set before time is inited!");
		return;
	}
#endif /*DEV*/

	if (numtimers >= MAX_TIMERS - 1) {
		logprintf("ERR: timer limit "EQ(MAX_TIMERS)" reached");
		return;
	}

	timer = timers + numtimers;
	timer->time_left = interval;
	timer->callback = cb;
	timer->data = data;
	numtimers++;
}

void timer_tick()
{
	struct TIMER *t;
	int i, elapsed_time, new_time;

	elapsed_time = time_elapsed_millis();
	for (i = 0, t = timers; i < numtimers; i++, t++) {
		t->time_left -= elapsed_time;
		if (t->time_left <= 0) {
			new_time = t->callback(t->data);
			if (new_time > 0) {
				t->time_left += new_time;
			} else {
				if (i != numtimers - 1) {
					memcpy(t,
						timers + numtimers - 1,
						sizeof(struct TIMER));
					i--;
					t--;
				}
				numtimers--;
			}
		}
	}
}
