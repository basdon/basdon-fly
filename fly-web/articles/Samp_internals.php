<ul>
	<li><a href=#client_console_commands>Client console commands (0.3.7-R4)</a>
</ul>

<h3 id=client_console_commands>Client console commands (0.3.7-R4)</h3>

<p>
	See <code>samp.dll+690D0</code>.

<ul>
	<li><code>/audiomsg</code> - toggles whether messages are printed when audio streams are played
	<li><code>/cmpstat</code> - does absolutely nothing
	<li><code>/ctd</code> - toggles camera target debug (what is the effect?)
	<li><code>/dl</code> - toggles debug labels on vehicles
	<li><code>/fontsize &lt;size&gt;</code> - changes chat window's font size
	<li><code>/fpslimit &lt;limit&gt;</code> - set fps limit of the game. only has effect if "Frame limiter" in game settings is enabled.
	<li><code>/headmove</code> - toggles whether other players' character head will move to the direction they're looking at
	<li><code>/hudscalefix</code> - toggles a fix that ensures the minimap radar is a circle on widescreen resolutions
	<li><code>/interior</code> - prints the current game interior
	<li><code>/logurls</code> - toggles whether messages are printend when files are downloaded. file downloading is leftover code that is not usable.
	<li><code>/mem</code> - prints the game's streaming memory limit
	<li><code>/nametagstatus</code> - toggles whether the "hourglass" icon is shown next to players' nametags when they are paused
	<li><code>/pagesize &lt;size&gt;</code> - sets amount of lines that the chat window shows on screen
	<li><code>/q</code> - alias of <code>/quit</code>
	<li><code>/quit</code> - quit the game
	<li><code>/rcon &gt;command&lt;</code> - sends rcon commands to the server
	<li><code>/rs [comment]</code> - saves current <code>x,y,z,facingAngle</code> position in <code>Documents\GTA San Andreas User Files\SAMP\rawpositions.txt</code>
	<li>
		<code>/save [comment]</code> - saves current position in <code>Documents\GTA San Andreas User Files\SAMP\savedpositions.txt</code> formatted as a
		<a href=https://basdon.github.io/documented-samp-pawn-api/main.xml#AddPlayerClass><code>AddPlayerClass</code></a><img src=/s/moin-www.png alt="globe icon" title="external link"> or
		<a href=https://basdon.github.io/documented-samp-pawn-api/main.xml#AddStaticVehicle><code>AddStaticVehicle</code></a><img src=/s/moin-www.png alt="globe icon" title="external link"> function call, depending on your state.
	<li><code>/testdw</code> - send test values to the death message list (death message list may be hidden with <kbd>F9</kbd>)
	<li><code>/timestamp</code> - toggles whether to show timestamp before chat messages
	<li><code>/togobjlight</code> - seems unused
</ul>

When launching SA-MP Debug (<code>samp_debug.exe</code>), following commands are also available:

<ul>
	<li><code>/player_skin &lt;skinid&gt;</code>
	<li><code>/set_weather &lt;weatherid&gt;</code>
	<li><code>/set_time &lt;hour&gt; &lt;minute&gt;</code>
	<li><code>/v &lt;model&gt;</code> - alias of <code>/vehicle</code>
	<li><code>/vehicle &lt;modelid&gt;</code> - spawns a vehicle next to you. <code>model</code> value is unchecked, so this will crash the client if it's invalid
	<li><code>/vsel</code> - opens vehicle selection UI. Use the <code>JUMP</code>/<code>SPRINT</code> keys to select the model and <code>ENTER VEHICLE</code> key to spawn
</ul>
