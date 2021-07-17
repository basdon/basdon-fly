<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/__settings.php';
require '../templates/missiontypes.php';
$classes = [];
$classes[$SETTING__CLASSID_PASSENGER_PILOT] = [$SETTING__PASSENGER_PILOT_NAME, $SETTING__PASSENGER_PILOT_MSPTYPES];
$classes[$SETTING__CLASSID_CARGO_PILOT] = [$SETTING__CARGO_PILOT_NAME, $SETTING__CARGO_PILOT_MSPTYPES];
$classes[$SETTING__CLASSID_RESCUE] = [$SETTING__RESCUE_NAME, $SETTING__RESCUE_MSPTYPES];
$classes[$SETTING__CLASSID_ARMY] = [$SETTING__ARMY_NAME, $SETTING__ARMY_MSPTYPES];
$classes[$SETTING__CLASSID_AID] = [$SETTING__AID_NAME, $SETTING__AID_MSPTYPES];
if (count($classes) != $SETTING__NUM_CLASSES) {
	echo "missing classes!\n";
	exit(1);
}
ob_start();
?>
<h3>Mission type availability per class</h3>
<style>
#mpta th,#mpta td {
	padding: .2em .4em;
}
#mpta td, #mpta th {
	text-align: center;
}
#mpta th,#mpta tr>td:first-child {
	background: #dc6;
	text-align: left;
}
#mpta td.y {
	background: #6d6;
}
#mpta td.n {
	background: #d66;
}
</style>
<div>
	<div class="tablescroller">
		<table id="mpta" class="new rowseps">
			<thead>
				<tr>
					<th></th>
					<?php foreach ($classes as $c) { ?>
						<th><?=$c[0]?></th>
					<?php } ?>
				</tr>
			</thead>
			<tbody>
				<?php for ($i = 0; $i < $SETTING__NUM_MISSION_TYPES; $i++) { ?>
					<tr>
						<td><?=fmt_mission_type(1 << $i)?></td>
						<?php foreach ($classes as $c) { ?>
							<?php if ($c[1] & (1 << $i)) { ?>
								<td class="y">yes</td>
							<?php } else { ?>
								<td class="n">no</td>
							<?php } ?>
						<?php } ?>
					</tr>
				<?php } ?>
			</tbody>
		</table>
	</div>
</div>
<?php
$htmldata = ob_get_clean();
$htmldata = str_replace("\t", "", $htmldata);
$htmldata = str_replace("\n", "", $htmldata);
file_put_contents('../static/gen/mission-point-types-classes.html', $htmldata);
