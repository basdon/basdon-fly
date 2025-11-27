<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/__settings.php';
require '../inc/missiontypes.php';
require '../inc/vehiclenames.php';
$vehicles = [
	[592, $SETTING__ANDROM_MSPTYPES],
	[577, $SETTING__AT400_MSPTYPES],
	[511, $SETTING__BEAGLE_MSPTYPES],
	[548, $SETTING__CARGOBOB_MSPTYPES],
	[512, $SETTING__CROPDUST_MSPTYPES],
	[593, $SETTING__DODO_MSPTYPES],
	[425, $SETTING__HUNTER_MSPTYPES],
	[520, $SETTING__HYDRA_MSPTYPES],
	[417, $SETTING__LEVIATHN_MSPTYPES],
	[487, $SETTING__MAVERICK_MSPTYPES],
	[553, $SETTING__NEVADA_MSPTYPES],
	[497, $SETTING__POLMAV_MSPTYPES],
	[563, $SETTING__RAINDANC_MSPTYPES],
	[476, $SETTING__RUSTLER_MSPTYPES],
	[447, $SETTING__SEASPAR_MSPTYPES],
	[519, $SETTING__SHAMAL_MSPTYPES],
	[460, $SETTING__SKIMMER_MSPTYPES],
	[469, $SETTING__SPARROW_MSPTYPES],
	[513, $SETTING__STUNT_MSPTYPES],
	[488, $SETTING__VCNMAV_MSPTYPES],
];
ob_start();
?>
<h3>Mission type availability per vehicle</h3>
<style>
#mpta th,#mpta td {
	padding: .2em .4em;
	text-align: center;
	white-space: nowrap;
}
#mpta tr>td:first-child {
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
					<?php foreach ($vehicles as $v) { ?>
						<th>
							<a href="/article.php?title=List_of_aircraft#<?=str_replace(' ', '_', vehicle_name($v[0]))?>">
								<?=vehicle_name($v[0])?>
							</a>
						</th>
					<?php } ?>
				</tr>
			</thead>
			<tbody>
				<?php for ($i = 0; $i < $SETTING__NUM_MISSION_TYPES; $i++) { ?>
					<tr>
						<td><?=fmt_mission_type(1 << $i)?></td>
						<?php foreach ($vehicles as $v) { ?>
							<?php if ($v[1] & (1 << $i)) { ?>
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
file_put_contents('../www/s/gen/mission-point-types-vehicles.html', $htmldata);
