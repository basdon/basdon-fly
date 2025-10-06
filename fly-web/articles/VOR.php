<p>
	VHF (Very High Frequency) Omni-directional Range, or VOR, is a navigation
	system for aircfraft. The main difference with <?php link_article('ADF'); ?>
	is that VOR guides towards the beacon from a specific direction, or radial.

<h3>Usage</h3>

<pre>/vor &lt;beacon&gt;[ ]&lt;angle&gt;[specifier]

examples:

/vor salv 18
/vor SALV18
/vor sals 27L
/vor SALS09R
</pre>

<p>
	Beacons and specifiers can be in uppercase or lowercase or even mixed. A space
	between the beacon and the angle is optional.
<p>
	<code>&lt;beacon&gt;</code> is the code of the NDB beacon to use.
	<code>&lt;angle&gt;</code> is the angle to use. <code>[specifier]</code> can be a runway
	specifier, like <code>R</code> or <code>L</code> or <code>C</code>. See
	<?php link_article('List_Of_Beacons'); ?> for a list of all the available beacons with their
	angles and specifiers.
<p>
	When ingame, the <code>/nearest</code> or <code>/beacons</code>
	commands can be used. To find out the available angles ingame, click on an entry
	in the <code>/nearest</code> list or just type the <code>/vor</code>
	command without specifying the angle and a message will show with the available angles.

<pre>/vor salv
! Unknown runway, try one of: 18 36

/vor savm
! There are no VOR capable runways at this beacon
</pre>

<p>
	To disable the navigation, simply type <code>/vor</code> again (or <code>/adf</code>).

<h3>Panel</h3>

<p>
	When VOR is engaged, values will be shown in the navigation top part of the
	panel, similar to ADF (but the <code>CRS</code> has a different meaning!). Near the
	bottom another bar shows up, indicating the alignment with the runway.
<figure>
	<img class=fullsize src=/s/articles/upd200922panel.png alt="a wide box with lots of numbers indicating speed, altitude, heading, vertical air speed, navigation stuff" title="panel">
	<figcaption>The aircraft panel with VOR navigation engaged</figcaption>
</figure>
<p>
	Indicators and their meanings:
<dl>
	<dt>
		DIS
	<dd>
		Horizontal distance between the aircraft and the beacon.
	<dt>
		ALT
	<dd>
		Difference in altitude. A positive number indicates the aircraft is at a
		higher altitude than the beacon, a negative number indicated the opposite.
	<dt>
		CRS
	<dd>
		Difference in the aircraft's course and the radial. If this is 0, it means
		the aircraft is flying parallel to the runway.
</dl>
<p>
	The VOR bar at the bottom indicates where the runway position is relative to
	the aircraft's position. If the alignment is too far off, a number is shown,
	giving an idea of how far off the aircraft is.

<h3>Examples</h3>

<p>
	When <code>CRS</code> is 0 and the vertical bar in the VOR bar is centered, the
	aircraft is heading directly towards the runway.
<figure>
	<img class=fullsize src=/s/articles/vor1.png alt="a plane going towars a runway, which is straight ahead" title="VOR example 1">
	<figcaption>VOR example 1</figcaption>
</figure>
<p>
	When the vertical bar is not centered while the <code>CRS</code> is 0, the aircraft
	is going in the right direction, but not in the center of the runway.
<figure>
	<img src=/s/articles/vor2.png alt="a plane going towards a runway, but the runway is moreto the right" title="VOR example 2">
	<figcaption>VOR example 2</figcaption>
</figure>
<p>
	When the aircraft is too far from the centerline, the bar will change into a
	number, giving an idea of how far exactly the aircraft is off the centerline.
<figure>
	<img src=/s/articles/vor3.png alt="a plane going towards a runway, but the runway is a lotmore to the right" title="VOR example 3">
	<figcaption>VOR example 3</figcaption>
</figure>
<p>
	When the <code>CRS</code> is not close to 0, the aircraft is not going parallel with
	the runway. The VOR bar still shows how far the aircraft is from the centerline,
	as if it was going parallel.
<figure>
	<img src=/s/articles/vor4.png alt="a plane going towards a runway which is ahead but at anangle" title="VOR example 4">
	<figcaption>VOR example 4</figcaption>
</figure>
<p>
	If the <code>CRS</code> is more than 90 or less than -90, the aircraft is going away
	from the runway. In this case, the VOR bar will show at the opposite side, since
	it's still being calculated as if the aircraft would be going parallel.
<figure>
	<img src=/s/articles/vor5.png alt="a plane going towards a runway, but the runway is at the right side and is angled towards the bottom right" title="VOR example 5">
	<figcaption>VOR example 5</figcaption>
</figure>
