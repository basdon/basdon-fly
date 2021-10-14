/*arbitrary value*/
#define MAX_TIMERS (500 + MAX_PLAYERS * 10)
#define TIMER_NO_REPEAT (0)

typedef int (*timer_cb)(void* data);

struct TIMER {
	int time_left;
	timer_cb callback;
	void *data;
};

static struct TIMER timers[MAX_TIMERS];
static int numtimers;

/**
Resets timer checking, to be called at the end of OnGameModeInit.

Otherwise the first tick may immediately jump a few seconds and may already hit
timers.
*/
static
void timer_reset()
{
	time_elapsed_millis();
}

/**
Set a timer.

Don't forget to free data if needed when applicable.

@param data data to pass to timer_cb on hit
*/
static
void timer_set(int interval, timer_cb cb, void *data)
{
	struct TIMER *timer;

#ifdef DEV
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

/**
Process the timers.
*/
static
void timer_tick(int elapsed_time)
{
	struct TIMER *t;
	int i, new_time;

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
