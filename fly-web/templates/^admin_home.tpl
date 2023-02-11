<h2 id="main">Admin</h2>
<h3>Health checks</h3>
<ul>
	<li>playtime &gt; onlinetime: <span class="indicator indicator-value-{@unsafe $healthchecks->playtime}">{@unsafe $healthchecks->playtime}</span></li>
	<li>flighttime &gt; playtime: <span class="indicator indicator-value-{@unsafe $healthchecks->flighttime}">{@unsafe $healthchecks->flighttime}</span></li>
	<li>distance &gt; 10000000: <span class="indicator indicator-value-{@unsafe $healthchecks->distance}">{@unsafe $healthchecks->distance}</span></li>
</ul>
<h3>Heartbeat</h3>
<ul>
	<li>
		Server uptime:&#32;
		{@if empty($heartbeat)}
			<strong class="indicator">OFFLINE</strong>
		{@else}
			{@unsafe format_duration_short_days(time() - $heartbeat[0]->tstart)}&#32;
			({@unsafe format_datetime($heartbeat[0]->tstart)})
			{@if time() - $heartbeat[0]->tstart > 86400 * 20}
				&#32;<span style="color:#ea0">restart suggested</span>
			{@endif}
		{@endif}
	</li>
	<li>Last heartbeat: {@if empty($lastheartbeat)}<strong class="indicator">NONE?</strong>{@else}{@unsafe format_duration_short_days(time() - $lastheartbeat[0]->tlast)} ago{@endif}</li>
</ul>
<h4>Last sessions</h4>
<table class="new center rowseps">
	<thead><tr><th>start<th>end<th>duration<th>cleanexit</thead>
	<tbody>
		{@foreach fetch_last_sessions_info() as $s}
			{@if isset($s->offline_time_s)}
				<tr>
					<td colspan=4>
						{@if $s->offline_time_s > 90}<strong>{@endif}
						offline for {@unsafe format_duration_short_days($s->offline_time_s)}
						{@if $s->offline_time_s > 90}</strong>{@endif}
			{@endif}
			<tr>
				<td>{@unsafe format_datetime($s->tstart)}</td>
				<td>{@unsafe format_datetime($s->tlast)}</td>
				<td>{@unsafe format_duration_short_days($s->tlast - $s->tstart)}</td>
				<td>{@unsafe $s->cleanexit}</td>
		{@endforeach}
	</tbody>
</table>
