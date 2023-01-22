{@rem needed variables: $name (user name) $id (user id) $flight_list @}

<h2 id="main">Profile of {$name} ({@unsafe $id})</h2>
{@try}
	{@eval ++$db_querycount}
	{@eval $r = $db->query('SELECT registertime,lastseengame,score,onlinetime,playtime,flighttime,distance,cash FROM usr WHERE i=' . $id . ' LIMIT 1')}
	{@if $r !== false && ($r = $r->fetchAll()) !== false && count($r)}
		{@eval $r = $r[0]}
		<ul>
			<li><strong>Score:</strong> {@unsafe $r->score}</li>
			<li><strong>Online time:</strong> {@unsafe format_duration_short($r->onlinetime)}</li>
			<li><strong>Play time:</strong> {@unsafe format_duration_short($r->playtime)}</li>
			<li><strong>Flight time:</strong> {@unsafe format_duration_short($r->flighttime)}</li>
			<li><strong>Distance flown:</strong> {@unsafe round($r->distance, 1)}km</li>
		</ul>
		<ul>
			<li><strong>Registered since:</strong> {@unsafe date('j M Y H:i', $r->registertime)} ({@unsafe format_time_since($r->registertime)})</li>
			<li><strong>Last seen: </strong>
				{@if time() - $r->lastseengame < 50}
					<strong style="color:#0b0">online now</strong>
				{@else}
					{@unsafe date('j M Y H:i', $r->lastseengame)} ({@unsafe format_time_since($r->lastseengame)})
				{@endif}
			</li>
		</ul>
		<ul>
			{@eval ++$db_querycount}
			{@eval $flg = $db->query('SELECT
						  (SELECT COUNT(id) FROM flg WHERE player='.$id.' AND state!=1 AND state!=64) totalflights,
						  (SELECT COUNT(id) FROM flg WHERE player='.$id.' AND state=8) finishedflights')}
			{@if $flg !== false && ($flg = $flg->fetchAll()) !== false && count($flg)}
				{@eval $flg = $flg[0]}
				<li><strong>Finished flights:</strong> {@unsafe $flg->finishedflights}</li>
				<li><strong>Total flights:</strong> {@unsafe $flg->totalflights}</li>
			{@endif}
			{@eval ++$db_querycount}
			{@eval $avgpst = $db->query('SELECT AVG(satisfaction) a,COUNT(id) c FROM flg WHERE player=' . $id . ' AND missiontype&' . $SETTING__PASSENGER_MISSIONTYPES)}
			{@if $avgpst !== false && ($avgpst = $avgpst->fetchAll()) !== false && count($avgpst) && ($avgpst = $avgpst[0]->a) != null}
				<li><strong>Average passenger satisfaction:</strong> {@unsafe round($avgpst, 2)}%</li>
			{@endif}
		</ul>
		<ul>
			{@eval ++$db_querycount}
			{@eval $rp = $db->query('SELECT SUM(paid) s FROM refuellog WHERE invokr='.$id)}
			{@if $rp !== false && ($rp = $rp->fetchAll()) !== false && count($rp)}
				<li><strong>Refuel budget:</strong> {@unsafe fmt_money(-$rp[0]->s)}</li>
			{@endif}
			{@eval ++$db_querycount}
			{@eval $rp = $db->query('SELECT SUM(paid) s FROM repairlog WHERE invokr='.$id)}
			{@if $rp !== false && ($rp = $rp->fetchAll()) !== false && count($rp)}
				<li><strong>Repair budget:</strong> {@unsafe fmt_money(-$rp[0]->s)}</li>
			{@endif}
			<li><strong>Money in hand:</strong> {@unsafe fmt_money($r->cash)}</li>
		</ul>
		<h3>Sessions</h3>
		{@eval ++$db_querycount}
		{@eval $ls = $db->query('SELECT s,e FROM ses WHERE u='.$id.' ORDER BY e-s DESC LIMIT 1')}
		{@if $ls !== false && ($ls = $ls->fetchAll()) !== false && count($ls)}
			{@eval $len = round(($ls[0]->e - $ls[0]->s) / 60)}
			{@eval $lenm = $len % 60}
			{@eval $lenh = floor(($len - $lenm) / 60)}
			<p>Longest in-game session: {@if $lenh > 0}{@unsafe $lenh}h, {@endif}{@unsafe $lenm}m set on {@unsafe date('l jS \of F, Y', $ls[0]->s)}.</p>
		{@endif}

		<table class="new center">
			<thead>
				<tr><th>Session start time</th><th>Session duration</th></tr>
			</thead>
			<tbody>
				{@eval $ls = $db->query('SELECT s,e FROM ses WHERE u='.$id.' ORDER BY i DESC LIMIT 5')}
				{@if $ls !== false}
					{@foreach $ls as $s}
						<tr>
							<td>
								{@unsafe date('F jS, Y - H:i', $s->s)}
							</td>
							<td>
								{@eval $len = round(($s->e - $s->s) / 60)}
								{@eval $lenm = $len % 60}
								{@eval $lenh = floor(($len - $lenm) / 60)}
								{@unsafe $lenh}h{@if $lenm < 10}0{@endif}{@unsafe $lenm}m
							</td>
						</tr>
					{@endforeach}
				{@endif}
			</tbody>
		</table>

		<h3>Last {@unsafe $flight_list->opts->limit} flights by this user</h3>
		{@render flightlist.tpl}
		<p class="center">
			<a href="/flights.php?{@unsafe $flight_list->active_filters_query_params}">
				All flights by this user
			</a>
		</p>
	{@endif}
{@catch PDOException $e}
	<p>Something went wrong while retrieving the user's data.</p>
{@endtry}
