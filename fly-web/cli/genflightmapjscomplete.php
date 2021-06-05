<?php
$complete = 'var fm_islands=';
$complete .= file_get_contents('../gen/islandmapdata.txt');
$complete .= file_get_contents('../static/flightmap.js');
$complete = str_replace("\t", "", $complete);
file_put_contents('../static/gen/fm_complete.js', $complete);
