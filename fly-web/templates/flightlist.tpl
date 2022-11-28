{@unsafe $pagination = simple_pagination($flightlist_pagination_url, $page, $totalrows, 50)}
{@if $flightlist !== false}
	{@render flightstatuses.php}
	{@render aircraftnames.php}
	<table border="0" width="100%" id="flights" class="flightlist center">
		<thead>
			<tr>
				<th width="1px">#</th>
				<th>Aircraft</th>
				{@if $flightlist_show_user}<th>Pilot</th>{@endif}
				<th>Route</th>
				<th width="1px">Status</th>
				<th class="right no800">Distance</th>
				<th>Date</th>
			</tr>
		</thead>
		<tbody>
			{@if $flightlist_url_returnpage}
				{@eval $rp = $page != 1 ? "&amp;rp=$page" : "";}
			{@endif}
			{@foreach $flightlist as $f}
				{@eval $aircraftname = aircraft_name($f->vehmodel)}
				<tr>
					<td><a href="flight.php?id={@unsafe $f->id}{@if $flightlist_url_returnpage}{@unsafe $rp}{@endif}">#{@unsafe $f->id}</a></td>
					<td style="padding:0">
						<img src="/s/s{@unsafe $f->vehmodel}.png" title="{@unsafe $aircraftname}" alt="{@unsafe $aircraftname}" />
					</td>
					{@if $flightlist_show_user}<td>{@unsafe linkuser($f)}</td>{@endif}
					<td class="route">
						<a href="article.php?title={@unsafe $f->f}">{@unsafe $f->f}</a>
						<span>&rarr;</span>
						<a href="article.php?title={@unsafe $f->t}">{@unsafe $f->t}</a>
					</td>
					<td><span class="flight-state flight-state-{@unsafe $f->state}">{@unsafe fmt_flight_status($f->state, $f->tload)}</span></td>
					<td class="right no800">
						{@if $f->state != 1}
							{@unsafe round($f->adistance)}m
						{@endif}
					</td>
					<td>{@unsafe date($flightlist_date_format, $f->tlastupdate)}</td>
				</tr>
			{@endforeach}
		</tbody>
	</table>
{@else}
	<p>None</p>
{@endif}
{@unsafe $pagination}
