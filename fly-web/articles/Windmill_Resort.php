<div class=articleright>
	<?php readfile('../gen/apfactsheet_WMRE.html') ?>
</div>

<p>
	This map was mapped by player Jenis.

<h3>Vehicle spawns</h3>

<?php readfile('../gen/apvehiclelist_WMRE.html') ?>

<h3>Service points</h3>

<p>
	There is one service point.
<figure>
	<img src=/s/articles/wmre_sp.jpg alt="Airport service point" title="Airport service point">
	<figcaption>Airport service point</figcaption>
</figure>

<!-- islandmap file is either empty or contains the h3 -->
<?php readfile('../gen/islandmap_WMRE.html') ?>

<h3>Aerodrome Chart</h3>

<?php readfile('../gen/aerodromechart_WMRE.html') ?>

<?php $code='WMRE'; include '../articles/airport-dyn-stats.php' ?>

<h3>Gallery</h3>

<img class=gallery src=/s/articles/wmre_golf.jpg title="Golf course" alt="Golf course">
<img class=gallery src=/s/articles/wmre_msp_tower.jpg title="Mission point and tower" alt="Mission point and tower">
<img class=gallery src=/s/articles/wmre_south.jpg title="South angle" alt="South angle">
