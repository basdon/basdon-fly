{@unsafe $pagination = simple_pagination($flightlist_pagination_url, $page, floor($totalrows / 50) + 1)}
{@if $flightlist !== false}
	{@render flightstatuses.php}
	{@render aircraftnames.php}
	<table border="0" width="100%" id="flights">
		<thead>
			<tr>
				<th>Date</th>
				{@if $flightlist_show_user}<th>Pilot</th>{@endif}
				<th>Aircraft</th>
				<th>Departure</th>
				<th>Arrival</th>
				<th>Distance</th>
				<th>Status</th>
				<th>Details</th>
			</tr>
		</thead>
		<tbody>
			{@if $flightlist_url_returnpage}
				{@eval $rp = $page != 1 ? "&rp=$page" : "";}
			{@endif}
			{@foreach $flightlist as $f}
				<tr>
					<td>{@unsafe date($flightlist_date_format, $f->tlastupdate)}</td>
					{@if $flightlist_show_user}<td><a href="user.php?name={@urlencode $f->n}">{$f->n}</a></td>{@endif}
					<td>{@unsafe aircraft_name($f->vehmodel)}</td>
					<td><a href="airport.php?code={@unsafe $f->f}">{@unsafe $f->f}</a></td>
					<td><a href="airport.php?code={@unsafe $f->t}">{@unsafe $f->t}</a></td>
					<td>{@if $f->state != 1}{@unsafe round($f->adistance)}m{@endif}</td>
					<td class="flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
					<td><a href="flight.php?id={@unsafe $f->id}{@if $flightlist_url_returnpage}{@unsafe $rp}{@endif}">Flight #{@unsafe $f->id} details</a></td>
				</tr>
			{@endforeach}
		</tbody>
	</table>
{@else}
	<p>None</p>
{@endif}
{@unsafe $pagination}
