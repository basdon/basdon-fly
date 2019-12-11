{@unsafe $pagination = simple_pagination($flightlist_pagination_url, $page, $totalrows, 50)}
{@if $flightlist !== false}
	{@render flightstatuses.php}
	{@render aircraftnames.php}
	<table border="0" width="100%" id="flights" class="default center">
		<thead>
			<tr>
				<th>#</th>
				<th>Date</th>
				{@if $flightlist_show_user}<th>Pilot</th>{@endif}
				<th class="no600">Aircraft</th>
				<th>From</th>
				<th>To</th>
				<th class="no800">Distance</th>
				<th>Status</th>
			</tr>
		</thead>
		<tbody>
			{@if $flightlist_url_returnpage}
				{@eval $rp = $page != 1 ? "&rp=$page" : "";}
			{@endif}
			{@foreach $flightlist as $f}
				<tr>
					<td><a href="flight.php?id={@unsafe $f->id}{@if $flightlist_url_returnpage}{@unsafe $rp}{@endif}">#{@unsafe $f->id}</a></td>
					<td>{@unsafe date($flightlist_date_format, $f->tlastupdate)}</td>
					{@if $flightlist_show_user}<td>{@unsafe linkuser($f)}</td>{@endif}
					<td class="no600">{@unsafe aircraft_name($f->vehmodel)}</td>
					<td><a href="airport.php?code={@unsafe $f->f}">{@unsafe $f->f}</a></td>
					<td><a href="airport.php?code={@unsafe $f->t}">{@unsafe $f->t}</a></td>
					<td class="no800 right">{@if $f->state != 1}{@unsafe round($f->adistance)}m{@endif}</td>
					<td class="flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</td>
				</tr>
			{@endforeach}
		</tbody>
	</table>
{@else}
	<p>None</p>
{@endif}
{@unsafe $pagination}
