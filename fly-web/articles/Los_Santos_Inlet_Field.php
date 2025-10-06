<div class=articleright>
	<?php readfile('../gen/apfactsheet_LSIF.html') ?>
</div>

<p>
	Los Santos Inlet Field is a small airfield located in the Los Santos Inlet area.
	It is almost soley used by recreational pilots and farmers. Since it's only a
	grass field, almost all the traffic is made of
	<?php link_article('List_of_aircraft', '#Dodo', true) ?>Dodo</a>s and
	<?php link_article('List_of_aircraft', '#Cropduster', true) ?>Cropduster</a>s.

<h3>Vehicle spawns</h3>

<?php readfile('../gen/apvehiclelist_LSIF.html') ?>

<h3>Service points</h3>

<p>
	LSIF is a small airfield that doesn't have any service points. The closest service
	point is probably located at <?php link_article('Los_Santos_Airport'); ?>.

<!-- islandmap file is either empty or contains the h3 -->
<?php readfile('../gen/islandmap_LSIF.html') ?>

<h3>Aerodrome Chart</h3>

<?php readfile('../gen/aerodromechart_LSIF.html') ?>

<?php $code='LSIF'; include '../articles/airport-dyn-stats.php' ?>
