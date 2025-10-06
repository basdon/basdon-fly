<ul>
	<li><a href=#Panel>Panel</a>
	<li><a href=#GPS>GPS</a>
	<li><a href=#Passenger_Satisfaction>Passenger satisfaction</a>
	<li><a href=#Kneeboard>Kneeboard</a>
	<li><a href=#Meters>Fuel/Health/ODO</a>
	<li><a href=#Speedo>Speedometer</a>
</ul>

<h3 id=Panel>Panel</h3>

<p>
	The 'panel' is a simple name for the box of information that shows up when
	inside an aircraft. It's placed at the bottom of the screen.
<p>
	At the left side of the panel, there is an airspeed indicator. The actual value
	is the big number in the vertical center, which shows the speed in knots. The
	other numbers are just part of the indicator, trying to make it look like a
	rotating system of numbers.
<p>
	At the right side of the panel, there is an altitude indicator and a vertical
	speed indicator. The altitude indicator is built in the same way as the
	airspeed indicator. The vertical speed indicator is a small vertical bar with
	a triangle indicator. When the indicator is above the center, the aircraft is
	gaining altitude. When the indicator is beneath the center, the aircraft is
	descending.
<p>
	The heading bar is located at the bottom of the panel. The bigger, white
	number is the current heading. The numbers to the side represent the heading
	the aircraft would be going when turning to that direction.
<p>
	At the top of the panel, there are are indicators used for
	<?php link_article_cat('Navigation'); ?>.
	They don't display anything unless navigation modes are activated.

<figure>
	<img src=/s/articles/hud_panel.png alt="a wide box with lots of numbers indicating speed, altitude, heading, vertical air speed, navigation stuff" title=panel style=max-width:unset>
	<figcaption>The aircraft panel</figcaption>
</figure>

<h3 id=GPS>GPS</h3>

<p>
	Above the radar at the bottom left of the screen, your current location is
	shown. This is referred to as the GPS. The bottom text displays the region you
	are in. If you are in a specific part (zone) of a region, it will show above
	the region text.
<p>
	The following image was taking in the Las Venturas region,
	near the zone around The Visage.
<figure>
	<img src=/s/articles/hud_gps.png alt="two texts above each other, showing above the game's radar" title="GPS text">
	<figcaption>GPS text</figcaption>
</figure>

<h3 id=Passenger_Satisfaction>Passenger satisfaction</h3>

<p>
	When doing a mission (see <?php link_article('Missions'); ?>) that involves transporting one or
	more passengers, the current <?php link_article('Passenger_Satisfaction'); ?> value will be
	shown underneath the radar, at the bottom left of the screen.
<figure>
	<img src=/s/articles/hud_satisfaction.png alt="Satisfaction text with percentage" title="passenger satisfaction">
	<figcaption>Passenger satisfaction text</figcaption>
</figure>

<h3 id=Kneeboard>Kneeboard</h3>

<p>
	The kneeboard is a small box to the right of the screen, showing information
	about the current mission. When not on a mission, it tells how to start a
	mission. When on a mission, it shows the origin and destination, with the next
	step highlighted in red. At the end it shows the distance towards the location
	to go towards.
<figure>
	<img src=/s/articles/hud_kneeboard_a.png alt="a box with title 'Kneeboard', showing the text 'Not on a job, type /w to start working.'" title="Kneeboard when not on a mission">
	<figcaption>Kneeboard when<br/>not on a mission</figcaption>
</figure><figure>
	<img src=/s/articles/hud_kneeboard_b.png alt="a box with title 'Kneeboard', showing the text 'Flight from Los Santos Airport to Las Venturas Airport. Distance: 233m' The 'Los Santos Airport' part is highlighted in red." title="Kneeboard when not loaded yet">
	<figcaption>Kneeboard when not<br/>loaded yet</figcaption>
</figure><figure>
	<img src=/s/articles/hud_kneeboard_c.png alt="a box with title 'Kneeboard', showing the text 'Flight from Los Santos Airport to Las Venturas Airport. Distance: 4.0Km The 'Las Venturas Airport' part is highlighted in red" title="Kneeboard when going towards the destination">
	<figcaption>Kneeboard when<br/>going towards destination</figcaption>
</figure>

<h3 id=Meters>Fuel/Health/ODO</h3>

<p>
	When in a vehicle, a small box shows at the bottom right of the screen. Inside
	the box, there's text showing the ODO, fuel level and health (points) of the
	vehicle.
<p>
	The ODO is the total distance that the vehicle has traveled, in Km. The
	fuel level and health indicators are horizontal bars, where the rightmost point
	means 100%. The leftmost point on the fuel bar means there's no fuel left, on
	the health bar it means that the vehicle is on fire and will explode.
<figure>
	<img src=/s/articles/hud_meters.png alt="text ODO with a number, FL and HP  with a horizontal bar" title="ODO/FL/HP meters">
	<figcaption>Meters</figcaption>
</figure>

<h3 id=Speedo>Speedometer</h3>

<p>
	When in a non-aircraft vehicle, a small speedometer text appears right above the
	meters box at the bottom right of the screen. It simply shows how fast the
	vehicle is going, in Km/h.
<figure>
	<img src=/s/articles/hud_speedo.png alt="a number text" title="speedometer text">
	<figcaption>The non-airvehicle<br/>speedometer</figcaption>
</figure>
