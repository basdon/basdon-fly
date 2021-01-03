{@rem needed variables: $name (user name) $id (user id) $paginationbaseurl (pagination base url, like user.php?id=xxx) @}

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
			{@render missiontypes.php}
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
			{@eval $avgpst = $db->query('SELECT AVG(satisfaction) a,COUNT(id) c FROM flg WHERE player=' . $id . ' AND missiontype&' . $passenger_mission_types)}
			{@if $avgpst !== false && ($avgpst = $avgpst->fetchAll()) !== false && count($avgpst) && ($avgpst = $avgpst[0]->a) != null}
				<li><strong>Average passenger satisfaction:</strong> {@unsafe round($avgpst, 2)}%</li>
			{@endif}
		</ul>
		<ul>
			{@render fmt_money.php}
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

		<h3>All flights by this user</h3>

		{@<?php}
			$page = get_page();
			$paginationoffset = ($page - 1) * 50;
			$totalrows=1;
			++$db_querycount;
			$amtflights = $db->query('SELECT COUNT(id) c FROM flg WHERE player='.$id);
			if ($amtflights !== false && ($amtflights = $amtflights->fetchAll()) !== false && count($amtflights)) {
				$totalrows = $amtflights[0]->c;
			}
		{@?>}

		{@eval ++$db_querycount}
		{@eval $flightlist = $db->query('SELECT id,_a.c f,_b.c t,state,tload,tlastupdate,adistance,_v.m vehmodel
						     FROM flg _f
						     JOIN apt _a ON _a.i=_f.fapt
						     JOIN apt _b ON _b.i=_f.tapt
						     JOIN veh _v ON _v.i=_f.vehicle
						     WHERE player='.$id.'
						     ORDER BY id DESC
						     LIMIT 50 OFFSET ' . $paginationoffset)}
		{@eval $flightlist_date_format = 'j M Y H:i'}
		{@eval $flightlist_show_user = 0}
		{@eval $flightlist_url_returnpage = 0}
		{@eval $flightlist_pagination_url = $paginationbaseurl.'&amp;page='}
		{@render flightlist.tpl}
	{@endif}
{@catch PDOException $e}
	<p>Something went wrong while retrieving the user's data.</p>
{@endtry}
