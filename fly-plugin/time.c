/**
Timestamp value when time_init was called.
*/
static unsigned long initial_value = 0;
/**
Timestamp value last time time_elapsed_millis was called.
*/
static unsigned long last_value;

/**
Get amount of elapsed milliseconds since time_init was called.
*/
static
unsigned long time_timestamp()
{
	unsigned long value;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	value = (unsigned long) (tv.tv_sec * 1000L);
	value += (unsigned long) (tv.tv_usec / 1000L);
	return value - initial_value;
}

/**
Inits time, must be called before using other time functions.
*/
static
void time_init()
{
	if (!initial_value) {
		initial_value = time_timestamp();
		last_value = 0;
	}
}

/**
Check if time_init has been called already.
*/
int time_is_inited()
{
	return initial_value;
}

/**
Get amount of elapsed milliseconds since last call.
*/
static
int time_elapsed_millis()
{
	unsigned long now;
	int diff;

	now = time_timestamp();
	diff = (int) (now - last_value);
	last_value = now;
	return diff;
}

/**
Sleep.
*/
static
void time_sleep(int millis)
{
	usleep(millis * 1000);
}
