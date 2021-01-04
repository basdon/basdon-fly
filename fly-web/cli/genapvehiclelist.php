<?php

require '../inc/conf.php';
require '../inc/db.php';
require '../templates/vehiclenames.php';

$apts = $db->query('SELECT i,c FROM apt')->fetchAll();

nextap:

$apt = array_pop($apts);
if ($apt == null) {
	exit();
}

$mdata = $db->query('SELECT MIN(m) AS model,COUNT(m) AS amt FROM veh WHERE ap=' . $apt->i . ' AND ownerplayer IS NULL GROUP BY m')->fetchAll();

ob_start();
if (count($mdata) == 0) {
	echo 'None!';
} else {
	echo '<ul>';
	foreach ($mdata as $i => $md) {
		echo '<li>' . vehicle_name($md->model);
		if ($md->amt > 1) {
			echo ' (' . $md->amt. ')';
		}
		echo '</li>';
	}
	echo '</ul>';
}
file_put_contents('../gen/apvehiclelist_'.$apt->c.'.html', ob_get_contents());
ob_end_clean();

goto nextap;
