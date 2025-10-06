<figure style=float:right>
	<img src=/s/articles/svplbl.jpg alt="Service point 3D text" title="Service point 3D text">
	<figcaption>Floating text label indicating a service point.</figcaption>
</figure>
<p>
	Service points are areas where you can repair and refuel your vehicle.
	They are marked on the radar with a S symbol:
	<img src=/s/articles/servicepoint.png alt="Sweet mission icon" title="Service point marker">.
	Beware: because of SA-MP limits, a service point map icon does not show unless
	you are close to it. If you are close enough, there will also be a floating text label.
<p>
	To repair your vehicle, use the command <strong>/repair</strong> or <strong>/fix</strong>.
<p>
	To refuel your vehicle, use the command <strong>/refuel</strong>.
	You can only refuel when the engine is turned off (see
	<?php link_article('Keys') ?> on how to turn your engine off).
<p>
	Aside from the driver, passengers can also repair and refuel vehicles. In
	that case, the passenger will pay the fee. The driver will receive a message
	when this happens.

<h3 class=clear>Repair cost</h3>

<p>
	The repair cost is <code>$150 + $2 * (missing health)</code>. Every vehicle has a
	maximum health of 1000 (they start burning at 250). So if your vehicle has 900
	health, repairing will cost $350. This means you'll pay at least $152 and at
	most $2150 (if you manage to get enough damage and then fix your vehicle before
	it explodes).
<p>
	If you don't have enough money, the vehicle will be partially repaired. The base
	$150 cost covers the visual damage (including damage to aircraft controls) and
	servicing fees, every $2 after that will fix one health point.

<h3>Refuel cost</h3>

<p>
	The refuel cost is <code>$75 + $10 * (missing fuel percentage)</code>. This means
	you'll pay at least $85 and at most $1075.
<p>
	If you don't have enough money, the vehicle will be refueled as much as your
	budget allows. The base $75 is paid for handling fees, every $10 after that
	will add one percentage of fuel.
<p>
	Note that not all aircraft have the same fuel capacity and usage, but the refuel
	price is determined by percentages to not financially ruin players if they
	choose to fly bigger aircraft.

<h3>See also</h3>

<ul>
	<li><?php link_article('Fuel'); ?>
	<li><?php link_article('Keys'); ?>
	<li><?php link_article('List_of_aircraft'); ?>
</ul>
