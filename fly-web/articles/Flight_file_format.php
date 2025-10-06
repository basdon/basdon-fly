<p>
	This is the specification of the <code>.flight</code> file format used to store
	mission flight data.

<pre>byte 0: version
byte 1-3: 'F', 'L', 'Y'
</pre>

<p>
	The rest is dependent on the spec version.
	Since v6, all flight files are always upgraded to the newest version.
<p>
	Versions:
<ul>
	<li>
		<a href=#v6>Version 6</a>
	<li>
		<a href=#old>Old versions</a>
		<ul>
			<li><a href=#v5>Version 5</a>
			<li><a href=#v4>Version 4</a>
			<li><a href=#v3>Version 3</a>
			<li><a href=#v2>Version 2</a>
			<li><a href=#v1>Version 1</a>
		</ul>
</ul>

<h3 id=v6>Version 6</h3>

<ul>
	<li>
		player name (length) has been removed from the header
	<li>
		vehicle model has been removed from the header
	<li>
		vehicle fuel capacity has been removed from the header, fuel level
		data in frames is now a percentage (as a value [0-1])
	<li>
		a <code>flags</code> field has been added to the header, so all flight
		files can have the same version and structure and this field determines
		which data is available as opposed to using the version field
	<li>
		frame <code>flags</code> now has a bit to mark a packet as being the
		first packet since resuming a flight (after the player rejoined the game).
		This bit was given meaning later, while v6 was already in use
</ul>

<pre>byte 4-7: mission id (DWORD)
byte 8-9: flags (WORD)
	right to left:
	bit 1: mission start packet received, if this is unset it
	       means that these flags were never set correctly
	bit 2: has_bank_and_pitch (this data was added in v4)
	bit 3: has_altitude (all versions have had altitude but
               it was corrupted in v4 so files that were made
               during v4's reign have this set to 0)
-----------
size 10 bytes

&gt; data (frames), repeated x times
byte 0-3: timestamp (DWORD)
byte 4: flags (BYTE)
	right to left:
	bit 1: is player paused
	bit 2: is engine on
	bit 3: is first packet since flight was paused
byte 5: passenger satisfaction% [0-100] (BYTE)
byte 6-7: aircraft speed, knots (WORD)
byte 8-9: aircraft altitude (WORD)
byte 10-11: health (WORD)
byte 12-15: fuel level% [0-1] (FLOAT)
byte 16-19: x pos (FLOAT)
byte 20-23: y pos (FLOAT)
byte 24-25: bank in degrees * 10 (WORD)
byte 26-27: pitch in degrees * 10 (WORD)
-----------
size 28 bytes
</pre>

<p>
	if there was an issue with a mission start packet:
<ul>
	<li><code>flags</code> will be <code>0</code>
</ul>

<h3 id=old>Old versions</h3>

<h4 id=v5>Version 5</h4>

<pre>byte 4-7: mission id (DWORD)
byte 8-11: vehicle fuel capacity (FLOAT)
byte 12-13: vehicle model id (WORD)
byte 14: player name length (BYTE)
byte 15-39: player name + 0 padding (BYTE[25])
-----------
size 40 bytes

&gt; data, repeated x times
byte 0-3: timestamp (DWORD)
byte 4: flags (right to left: bit 1: paused? bit 2: engine on?) (BYTE)
byte 5: passenger satisfaction (BYTE)
byte 6-7: aircraft speed, knots (WORD)
byte 8-9: aircraft altitude (WORD)
byte 10-11: health (WORD)
byte 12-15: fuel level (FLOAT)
byte 16-19: x pos (FLOAT)
byte 20-23: y pos (FLOAT)
byte 24-25: bank in degrees * 10 (WORD)
byte 26-27: pitch in degrees * 10 (WORD)
-----------
size 28 bytes
</pre>

<p>
	if there was an issue with a mission start packet:
<ul>
	<li>vehicle fuel capacity may be <code>0</code>
	<li>vehicle model id may be <code>0</code>
	<li>player name will be <code>?</code>
</ul>

<h4 id=v4>Version 4</h4>

<p>
	The format of version 4 is identical to <a href=#v5>Version 5</a>, but version 4 had
	a bug where the <code>altitude</code> value had bogus data (it was the same as
	<code>pitch10</code>). To distinguish between flight files generated with bogus
	altitude data and corrently generated flight files, the version was bumped.

<h4 id=v3>Version 3</h4>

<pre>byte 4-7: mission id (DWORD)
byte 8-11: vehicle fuel capacity (FLOAT)
byte 12-13: vehicle model id (WORD)
byte 14: player name length (BYTE)
byte 15-39: player name + 0 padding (BYTE[25])
-----------
size 40 bytes

&gt; data, repeated x times
byte 0-3: timestamp (DWORD)
byte 4: flags (right to left: bit 1: paused? bit 2: engine on?) (BYTE)
byte 5: passenger satisfaction (BYTE)
byte 6-7: aircraft speed, knots (WORD)
byte 8-9: aircraft altitude (WORD)
byte 10-11: health (WORD)
byte 12-15: fuel level (FLOAT)
byte 16-19: x pos (FLOAT)
byte 20-23: y pos (FLOAT)
-----------
size 24 bytes
</pre>

<p>
	if there was an issue with a mission start packet:
<ul>
	<li>vehicle fuel capacity may be <code>0</code>
	<li>vehicle model id may be <code>0</code>
	<li>player name will be <code>?</code>
</ul>

<h4 id=v2>Version 2</h4>

<p>
	Discontinued, existing files upgraded to v3.

<h4 id=v1>Version 1</h4>

<p>
	Discontinued, existing files upgraded to v2.

<h3>See also</h3>

<ul>
	<li><?php echo link_article('Flighttracker'); ?>
</ul>

