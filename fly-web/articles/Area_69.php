<div class=articleright>
	<?php readfile('../gen/apfactsheet_BASE.html') ?>
</div>

<p>
	Area 69 is a top secret government area located in the deserts of Bone County.
	It has a small runway at the eastern side, housing some
	<?php link_article('List_of_aircraft', '#Hydra', true); ?>Hydra</a>s,
	<?php link_article('List_of_aircraft', '#Rustler', true); ?>Rustler</a>s and
	<?php link_article('List_of_aircraft', '#Cargobob', true); ?>Cargobob</a>s
<p>
	There are four SAM sites at the facility, but they are inactive.

<h3>Vehicle spawns</h3>

<?php readfile('../gen/apvehiclelist_BASE.html') ?>

<h3>Service points</h3>
<p>
	There is one service point, towards the east of the runway.
<figure>
	<img src=/s/articles/base_sp.jpg alt="Area 69 service point" title="Area 69 service point">
	<figcaption>Area 69 service point</figcaption>
</figure>

<!-- islandmap file is either empty or contains the h3 -->
<?php readfile('../gen/islandmap_BASE.html') ?>

<h3>Aerodrome Chart</h3>

<?php readfile('../gen/aerodromechart_BASE.html') ?>

<?php $code='BASE'; include '../articles/airport-dyn-stats.php' ?>
