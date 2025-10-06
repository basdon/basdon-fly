<dl>
	<dt>
		DEV
	<dd>
		makes a development build
	<dt>
		LOG_SLOW_TICKS
	<dd>
		print to console and log when 20 ticks took over 120ms (ideal is 100ms)
	<dt>
		MAPS_LOG_STREAMING
	<dd>
		prints when a map is streamed in/out for a player to console and logfile
	<dt>
		NO_NC_PARAM_SIZE
	<dd>
		don't set the parameter stack size when calling <code>AMX_NATIVE_CALL</code>
		functions, reducing code. This flag is defined inside <code>natives.h</code>.
		So far only bcrypt has been seen requiring a correct number, so those
		those functions ignore this flag.
	<dt>
		PRINTQUERIES
	<dd>
		prints all queries being done to the console and logfile
	<dt>
		TEST_AIRPORT_LOADING
	<dd>
		prints all airports and their runways on init
	<dt>
		ZONES_DEBUG
	<dd>
		prints info while zones are being assigned to regions
</dl>
