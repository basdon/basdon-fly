<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

require '../inc/conf.php';
require '../inc/__settings.php';
require '../inc/db.php';
require '../templates/missiontypes.php';

$res = $db->query('SELECT c,(SELECT BIT_OR(t) FROM msp WHERE a=apt.i) t FROM apt ORDER BY c ASC');
if (!$res || !($apts = $res->fetchAll())) {
	echo "query failed\n";
	exit(1);
}

ob_start();
?>
<h3>Mission type availability per airport</h3>
<style>
#mpta th,#mpta td {
	padding: .2em .4em;
}
#mpta td, #mpta th {
	text-align: center;
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
					<?php foreach ($apts as $apt) { ?>
						<th>
							<a href="/article.php?title=<?=$apt->c?>">
								<?=$apt->c?>
							</a>
						</th>
					<?php } ?>
				</tr>
			</thead>
			<tbody>
				<?php for ($i = 0; $i < $SETTING__NUM_MISSION_TYPES; $i++) { ?>
					<tr>
						<td><?=fmt_mission_type(1 << $i)?></td>
						<?php foreach ($apts as $apt) { ?>
							<?php if ($apt->t & (1 << $i)) { ?>
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
file_put_contents('../www/s/gen/mission-point-types-airports.html', $htmldata);
