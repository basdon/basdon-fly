
/**
Inits time, must be called before using other time functions.
*/
void time_init();
/**
Check if time_init has been called already.
*/
int time_is_inited();
/**
Get amount of elapsed milliseconds since time_init was called.
*/
unsigned long time_timestamp();
/**
Get amount of elapsed milliseconds since last call.
*/
int time_elapsed_millis();
/**
Sleep.
*/
void time_sleep(int millis);