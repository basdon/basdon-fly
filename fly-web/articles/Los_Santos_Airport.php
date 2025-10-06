<div class=articleright>
	<?php readfile('../gen/apfactsheet_SALS.html') ?>
</div>

<p>
	Los Santos Airport was originally built as the airport for the National Guard of
	Los Santos. It was transformed into a civil airport in 1942. Since then, the
	terminals have seen millions of passengers passing through.
<p>
	The airport is located at the southern beach of Los Santos, bordered by the
	freeway, Verdant Bluffs, El Corona and Willowfied.
<p>
	There are two long runways at LSA, which are connected to each other in a
	U-shape. All runways have full navigation capabilities. There is only a single
	helipad at LSA.

<h3>Vehicle spawns</h3>

<?php readfile('../gen/apvehiclelist_SALS.html') ?>

<h3>Service points</h3>

<p>
	Los Santos airport has two service points available, one in the east and one in
	the west.
<figure>
	<img src=/s/articles/sals_sp_east.jpg alt="East service point" title="East service point">
	<figcaption>East service point</figcaption>
</figure><figure>
	<img src=/s/articles/sals_sp_west.jpg alt="West service point" title="West service point">
	<figcaption>West service point</figcaption>
</figure>

<!-- islandmap file is either empty or contains the h3 -->
<?php readfile('../gen/islandmap_SALS.html') ?>

<h3>Aerodrome Chart</h3>

<?php readfile('../gen/aerodromechart_SALS.html') ?>

<?php $code='SALS'; include '../articles/airport-dyn-stats.php' ?>
