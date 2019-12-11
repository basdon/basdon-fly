<!DOCTYPE html>
<html lang="en">
<head>
	<title>flights :: basdon.net aviation server</title>
	{@render defaulthead.tpl}
</head>
<body>
	{@render skip.tpl}
	{@render defaultheader.tpl}
	<div class="singlebody">
		<h2 id="main">Flights</h2>
		{@eval $page = get_page()}
		{@try}
			{@<?php}
				++$db_querycount;
				$r = $db->query('SELECT COUNT(id) c FROM flg');
				$paginationoffset = ($page - 1) * 50;
				$totalrows = 0;
				if ($r !== false && ($r = $r->fetchAll()) !== false && count($r)) {
					$totalrows = $r[0]->c;
				}
				++$db_querycount;
				$r = $db->query('SELECT id,_u.name,_u.i,_a.c f,_b.c t,state,tload,tlastupdate,adistance,_v.m vehmodel 
						 FROM flg _f 
						 JOIN usr _u ON _f.player=_u.i 
						 JOIN apt _a ON _a.i=_f.fapt 
						 JOIN apt _b ON _b.i=_f.tapt 
						 JOIN veh _v ON _v.i=_f.vehicle 
						 ORDER BY id DESC 
						 LIMIT 50 OFFSET ' . $paginationoffset);
			{@?>}
			{@eval $flightlist = $r}
			{@eval $flightlist_date_format = 'j M H:i'}
			{@eval $flightlist_show_user = 1}
			{@eval $flightlist_url_returnpage = 1}
			{@eval $flightlist_pagination_url = 'flights.php?page='}
			{@render flightlist.tpl}
		{@catch PDOException $e}
			<p>Failed to get flight list.</p>
		{@endtry}
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
