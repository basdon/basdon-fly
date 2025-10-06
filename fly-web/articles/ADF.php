<p>
	The Automatic Direction Finder, or ADF, helps finding the direction towards a
	specific beacon. It tells the direction between the beacon and the aircraft,
	the difference in altitude and the difference in current aircraft course and
	course towards the beacon. For a list of beacons, see
	<?php echo link_article('List_Of_Beacons'); ?>.

<h3>Usage</h3>

<pre>/adf &lt;beacon&gt;

examples:

/adf salv
/adf SALS
</pre>
<p>
	Beacons and specifiers can be in uppercase or lowercase or even mixed.
<p>
	<code>&lt;beacon&gt;</code> is where you should put the code of the NDB beacon.
	See <?php echo link_article('List_Of_Beacons'); ?> for a list of all the available
	beacons. You can also find beacon codes by using commands <strong>/nearest</strong>
	and <strong>/beacons</strong>.
<p>
	To disable the navigation, simply type <code>/adf</code> again.

<h3>Panel</h3>

<p>
	When ADF is engaged, values will be shown in the navigation top part of the panel.

<figure>
	<img src=/s/articles/adf.png title="panel" alt="a wide box with lots of numbers indicating speed, altitude, heading, vertical air speed, navigation stuff">
	<figcaption>The aircraft panel with ADF navigation engaged</figcaption>
</figure>

<p>
	Indicators and their meanings:
<dl>
	<dt>DIS</dt>
	<dd>
		Horizontal distance between the aircraft and the beacon.
	</dd>
	<dt>ALT</dt>
	<dd>
		Difference in altitude. A positive number indicates the aircraft is at a
		higher altitude than the beacon, a negative number indicated the opposite.
	</dd>
	<dt>CRS</dt>
	<dd>
		Difference in the aircraft's course and the course towards the beacon. If
		this is 0, it means the aircraft is going directly towards the beacon.
	</dd>
</dl>

<h4>CRS examples</h4>

<p>
	In the first image, the aircraft has a heading of 03 or 30&deg;. The course
	towards the beacon is 09, or 90&deg;. In this case, the <code>CRS</code> indicator
	will show a value of <code>60</code>.
<p>
	In the second image, the aircraft still has a heading of 03, or 30&deg;. This
	time the course towards the beacon is 33, or 330&deg;. Here, the <code>CRS</code>
	indicator will show <code>-60</code>.

<figure>
	<img src=/s/articles/adf1.png title="CRS example 1" alt="a compass, heading 03 on top, with a SALV beacon to the top-right">
	<figcaption>CRS example 1</figcaption>
</figure><figure>
	<img src="/s/articles/adf2.png" title="CRS example 2" alt="a compass, heading 03 on top, with a SALV beacon to the top-left">
	<figcaption>CRS example 2</figcaption>
</figure>
