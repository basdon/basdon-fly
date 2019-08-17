<aside>
	<div>
		<h5>Players (last 48h)</h5>
		<img src="{@unsafe $STATICPATH}/gen/playergraph.png" />
		{@try}
			{@eval ++$db_querycount}
			{@eval $players = []}
			{@eval $r = $db->query("SELECT _u.name,_u.score
			                        FROM ses _s
			                        JOIN usr _u ON _s.u=_u.i
			                        WHERE _s.e>UNIX_TIMESTAMP()-40")}
			{@if $r !== false && ($r = $r->fetchAll()) !== false}
				{@eval $players = $r}
			{@endif}
			<h6>Currently online ({@unsafe count($players)})</h6>
			<table border="0" width="100%">
				{@foreach $players as $p}
					<tr><td><a href="user.php?name={@urlencode $p->name}">{$p->name}</a></td><td>{@unsafe $p->score}</td></tr>
				{@endforeach}
			</table>
		{@catch PDOException $e}
			<h6>Currently online</h6>
			<p style="color:red">Failed to get online players</p>
		{@endtry}
	</div>
	<div>
		<h5>Active Flights</h5>
		{@try}
			{@eval ++$db_querycount}
			{@eval $r = $db->query("SELECT id,_u.name,_a.c f,_b.c t
		                                FROM flg _f
		                                JOIN usr _u ON _f.player=_u.i
		                                JOIN apt _a ON _a.i=_f.fapt
		                                JOIN apt _b ON _b.i=_f.tapt
		                                WHERE state=1
		                                ORDER BY tunload DESC
		                                LIMIT 10")}
			{@if $r !== false && ($r = $r->fetchAll()) !== false}
				{@if count($r)}
					<table border="0" width="100%">
						{@foreach $r as $f}
							<tr>
								<td><a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a></td>
								<td>{@unsafe $f->f} -&gt; {@unsafe $f->t}</td>
								<td><a href="user.php?name={@urlencode $f->name}">{$f->name}</a></td>
							</tr>
						{@endforeach}
					</table>
				{@else}
					<p>None</p>
				{@endif}
			{@endif}
		{@catch PDOException $e}
		{@endtry}
	</div>
	<div>
		<h5>Last 7 Finished Flights</h5>
		{@try}
			{@eval ++$db_querycount}
			{@eval $r = $db->query("SELECT id,_u.name,_a.c f,_b.c t
			                        FROM flg _f
			                        JOIN usr _u ON _f.player=_u.i
			                        JOIN apt _a ON _a.i=_f.fapt
			                        JOIN apt _b ON _b.i=_f.tapt
			                        WHERE state=8
			                        ORDER BY id DESC
			                        LIMIT 7")}
			{@if $r !== false && ($r = $r->fetchAll()) !== false}
				{@if count($r)}
					<table border="0" width="100%">
						{@foreach $r as $f}
							<tr>
								<td><a href="flight.php?id={@unsafe $f->id}">#{@unsafe $f->id}</a></td>
								<td>{@unsafe $f->f} -&gt; {@unsafe $f->t}</td>
								<td><a href="user.php?name={@urlencode $f->name}">{$f->name}</a></td>
							</tr>
						{@endforeach}
					</table>
				{@else}
					<p>None?</p>
				{@endif}
			{@endif}
		{@catch PDOException $e}
		{@endtry}
		<p><a href="flights.php">All flights</a></p>
	</div>
	<div>
		<h5>Members</h5>
		<table border="0" width="100%">
		{@try}
			{@eval ++$db_querycount}
			{@eval $r = $db->query("SELECT COUNT(s.i) c FROM ses s JOIN usr u ON s.i=u.i WHERE u.groups=0")}
			{@if $r !== false && ($r = $r->fetch()) !== false}
				<tr><td>Guest sessions:</td><td>{@unsafe $r->c}</td></tr>
			{@endif}
			{@eval ++$db_querycount}
			{@eval $r = $db->query("SELECT COUNT(i) c FROM usr WHERE groups!=0")}
			{@if $r !== false && ($r = $r->fetch()) !== false}
				<tr><td>Registered users:</td><td>{@unsafe $r->c}</td></tr>
			{@endif}
			{@eval ++$db_querycount}
			{@eval $r = $db->query("SELECT n FROM usr WHERE groups!=0 ORDER BY i DESC LIMIT 1")}
			{@if $r !== false && ($r = $r->fetch()) !== false}
				<tr><td>Latest:</td><td><a href="user.php?name={@urlencode $r->n}">{$r->n}</a></td></tr>
			{@endif}
		{@catch PDOException $e}
		{@endtry}
		</table>
	</div>
</aside>
