<!DOCTYPE html>
<html lang="en">
<head>
	<title>basdon.net aviation server :: {@if $id == -1}user not found{@else}profile of {$name} ({@unsafe $id}){@endif}</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div>
		<div id="main">
			{@if $id == -1}
				<h2>User not found</h2>
				<p>Note that profiles of guest accounts can only be found by using their user account id.</p>
			{@else}
				<h2>Profile of {$name} ({@unsafe $id})</h2>
				{@try}
					{@eval ++$db_querycount}
					{@eval $r = $db->query('SELECT r,l,s,t,a,f,dis FROM usr WHERE i=' . $id . ' LIMIT 1')}
					{@if $r !== false && ($r = $r->fetchAll()) !== false && count($r)}
						{@eval $r = $r[0]}
						<ul>
							<li><strong>Score:</strong> {@unsafe $r->s}</li>
							<li><strong>Play time:</strong> {@unsafe format_duration_short($r->a)}</li>
							<li><strong>Flight time:</strong> {@unsafe format_duration_short($r->f)}</li>
							<li><strong>Total online time:</strong> {@unsafe format_duration_short($r->t)}</li>
							<li><strong>Total distance flown:</strong> {@unsafe round($r->dis / 1000, 1)}km</li>
						</ul>
						<ul>
							<li><strong>Registered since:</strong> {@unsafe date('j M Y H:i', $r->r)} ({@unsafe format_time_since($r->r)})</li>
							<li><strong>Last seen: </strong>
								{@if time() - $r->l < 50}
									<strong style="color:#0b0">online now</strong>
								{@else}
									{@unsafe date('j M Y H:i', $r->l)} ({@unsafe format_time_since($r->l)})
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
					{@endif}
				{@catch PDOException $e}
					<p>Something went wrong while retrieving the user's data.</p>
				{@endtry}
			{@endif}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
