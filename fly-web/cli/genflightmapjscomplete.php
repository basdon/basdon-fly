<?php
$island_data = file_get_contents('../static/gen/islandmap_fm_islands.js');
$island_data .= file_get_contents('../static/flightmap.js');
$island_data = str_replace("\t", "", $island_data);
file_put_contents('../static/gen/fm_complete.js', $island_data);
