<!DOCTYPE html>
<?php 
$id = -1;
$name = '';
if (isset($_GET['id'])) {
	$id = (int) $_GET['id'];
	$id++;
	$id--;
	$r = $db->query('SELECT n FROM usr WHERE i=' . $id . ' LIMIT 1');
	if ($r !== false && ($r = $r->fetchAll()) !== false && count($r)) {
		$name = $r[0]->n;
	} else {
		$id = -1;
	}
} elseif (isset($_GET['name'])) {
	$stmt = $db->prepare('SELECT i,g,n FROM usr WHERE n=? LIMIT 1');
	$stmt->bindValue(1, $_GET['name']);
	if ($stmt->execute() && ($r = $stmt->fetchAll()) && count($r) && $r[0]->g) {
		$id = $r[0]->i;
		$name = $r[0]->n;
	}
}
?>
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
<?php 
try {
	$r = $db->query('SELECT r,l FROM usr WHERE i=' . $id . ' LIMIT 1');
?>
				{@if $r !== false && ($r = $r->fetchAll()) !== false && count($r)}
					{@eval $r = $r[0]}
					<ul>
						<li><strong>Registered since:</strong> {@unsafe date('j M Y H:i', $r->r)}</li>
						<li><strong>Last seen: </strong>
							{@if time() - $r->l < 50}
								<strong style="color:#0b0">online now</strong>
							{@else}
								{@unsafe date('j M Y H:i', $r->l)}
							{@endif}
						</li>
					</ul>
				{@endif}
<?php 
} catch (PDOException $e) {
?>
				<p>Something went wrong while retrieving the user's data.</p>
<?php 
}
?>
			{@endif}
		</div>
		{@render aside.tpl}
		<div class="clear"></div>
	</div>
	{@render defaultfooter.tpl}
</body>
</html>
