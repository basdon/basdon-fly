<div class=articleright>
	<?php readfile('../gen/apfactsheet_SFCA.html') ?>
</div>

<p>
	San Fierro Carrier is the 'airport' name of the naval amphibious assault ship
	docked in the Easter Basin Naval Station of San Fierro. The ship is not actually
	an aircraft carrier, but the locals name it as such since it can accommodate a
	small number of <?php link_article('List_of_aircraft', '#Hydra', true) ?>Hydra</a>s
	and <?php link_article('List_of_aircraft', '#Cargobob', true) ?>Cargobob</a>s.
<p>
	Aside from the flight deck at the surface, there are two more decks inside.
	Each of those decks provide storage area and house military hardware. The lower
	deck provides access to open waters through the back gate of the carrier. There
	is some space to dock a few small boats.
<p>
	Two aircraft lifts are present, one at the back and one at the side near the
	back. They connect the flight deck and the middle deck, so that aircrafts can
	be stored in the middle deck.
<p>
	There are also two inactive SAM sites.

<h3>Vehicle spawns</h3>

<?php readfile('../gen/apvehiclelist_SFCA.html') ?>

<h3>Service points</h3>

<p>
	The ship has no service points. Aircrafts in need for repair usually go to
	<?php link_article('Area_69'); ?>, but there are also some service points at
	<?php link_article('San_Fierro_Airport'); ?> for very urgent situations.

<!-- islandmap file is either empty or contains the h3 -->
<?php readfile('../gen/islandmap_SFCA.html') ?>

<h3>Aerodrome Chart</h3>

<?php readfile('../gen/aerodromechart_SFCA.html') ?>

<?php $code='SFCA'; include '../articles/airport-dyn-stats.php' ?>
