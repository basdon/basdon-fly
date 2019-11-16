
/* vim: set filetype=c ts=8 noexpandtab: */

typedef int (*timer_cb)(void* data);

/**
Resets timer checking, to be called at the end of OnGameModeInit.

Otherwise the first tick may immediately jump a few seconds and may already hit
timers.
*/
void timer_reset();
/**
Set a timer.

Don't forget to free data if needed when applicable.

@param timer_cb zero to stop the timer or a value that will be the new interval
@param data data to pass to timer_cb on hit
*/
void timer_set(int interval, timer_cb cb, void *data);
/**
Process the timers.
*/
void timer_tick();
