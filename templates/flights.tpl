<!DOCTYPE html>
<html lang="en">
<head>
	<title>basdon.net aviation server :: flights</title>
	{@render defaulthead.tpl}
	<style>
		#flights td{text-align:center}
		#flights td:nth-child(5){text-align:right}
	</style>
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Flights</h2>
<?php 
$page = 1;
if (isset($_GET['page'])) {
	$page = (int)($_GET['page']);
	$page++;
	$page--;
	$page = max($page, 1);
}
try {
	$r = $db->query('SELECT COUNT(id) c FROM flg');
	$paginationoffset = ($page - 1) * 50;
	$totalrows = 0;
	if ($r !== false && ($r = $r->fetchAll()) !== false && count($r)) {
		$totalrows = $r[0]->c;
	}
	++$db_querycount;
	$r = $db->query('SELECT id,_u.n,_a.c f,_b.c t,state,tload,tlastupdate,adistance,_v.m vehmodel 
	                 FROM flg _f 
	                 JOIN usr _u ON _f.player=_u.i 
	                 JOIN apt _a ON _a.i=_f.fapt 
	                 JOIN apt _b ON _b.i=_f.tapt 
	                 JOIN veh _v ON _v.i=_f.vehicle 
	                 ORDER BY id DESC 
	                 LIMIT 50 OFFSET ' . $paginationoffset);
	if ($r !== false && ($r = $r->fetchAll()) !== false) {
		?>
		{@unsafe $pagination = simple_pagination('flights.php?page=', $page, floor($totalrows / 50) + 1)}
		{@if count($r)}
			{@render flightstatuses.php}
			{@render aircraftnames.php}
			<table border="0" width="100%" id="flights">
				<thead>
					<tr>
						<th>Date</th>
						<th>Pilot</th>
						<th>Aircraft</th>
						<th>Departure</th>
						<th>Arrival</th>
						<th>Distance</th>
						<th>Status</th>
						<th>Details</th>
					</tr>
				</thead>
				<tbody>
					{@eval $rp = $page != 1 ? "&rp=$page" : "";}
					{@foreach $r as $f}
						<tr>
							<td>{@unsafe date('j M H:i', $f->tlastupdate)}</td>
							<td><a href="user.php?name={@urlencode $f->n}">{$f->n}</a></td>
							<td>{@unsafe aircraft_name($f->vehmodel)}</td>
							<td><a href="airport.php?code={@unsafe $f->f}">{@unsafe $f->f}</a></td>
							<td><a href="airport.php?code={@unsafe $f->t}">{@unsafe $f->t}</a></td>
							<td>{@if $f->state != 1}{@unsafe round($f->adistance)}m{@endif}</td>
							<td class="flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
							<td><a href="flight.php?id={@unsafe $f->id . $rp}">Flight #{@unsafe $f->id} details</a></td>
						</tr>
					{@endforeach}
				</tbody>
			</table>
		{@else}
			<p>None</p>
		{@endif}
		{@unsafe $pagination}
		<?php 
	}
} catch (PDOException $e) {}
?>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
