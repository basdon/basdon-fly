<!DOCTYPE html>
<html lang="en">
<head>
	<title>{@if $id == -1}user not found{@else}profile of {$name} ({@unsafe $id}){@endif} :: basdon aviation server</title>
	{@render defaulthead.tpl}
	<style>
		#flights td{text-align:center}
		#flights td:nth-child(5){text-align:right}
	</style>
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
						{@eval $flightlist_pagination_url = 'user.php?'.$userqueryparam.'&page='}
						{@render flightlist.tpl}
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
