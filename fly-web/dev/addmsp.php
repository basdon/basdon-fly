<!DOCTYPE html>
<html>
<body>
<?php
require '.ignored.tempdb.php';
if (isset($_GET['xyz'], $_GET['ap'])) {
	$apid = $db->query("SELECT i FROM apt WHERE c='{$_GET['ap']}'")->fetch(PDO::FETCH_OBJ)->i;
	$c = explode(',', $_GET['xyz']);
	$x = $c[0];
	$y = $c[1];
	$z = $c[2];
	$t = 0;
	for ($i = 1; $i < (1 << 20); $i<<=1) {
		if (isset($_GET["t$i"])) {
			$t |= $i;
		}
	}
	$db->exec("INSERT INTO msp(a,x,y,z,t) VALUES ($apid,$x,$y,$z,$t)");
	echo "<p>inserted {$db->lastInsertId()}</p>";
}
?>
<form action="addmsp.php" method="get">
<p>
<label>x,y,z <input type="text" name="xyz"/></label><br />
<label>SALV <input type="text" name="ap"/></label><br />
<label><input type="checkbox" name="t1" />  1 small passenger: dodo</label><br />
<label><input type="checkbox" name="t2" />  2 medium passenger: shamal,beagle</label><br />
<label><input type="checkbox" name="t4" />  4 big passenger: at-400,andro</label><br />
<label><input type="checkbox" name="t8" />  8 small cargo: beagle</label><br />
<label><input type="checkbox" name="t16" /> 16 medium cargo: nevada</label><br />
<label><input type="checkbox" name="t32" /> 32 big cargo: andro</label><br />
<label><input type="checkbox" name="t64" /> 64 heli: (news,police) maverick,raindance,leviathan,sparrow</label><br />
<label><input type="checkbox" name="t128" />128 heli cargo: leviathan</label><br />
<label><input type="checkbox" name="t256" />256 military heli: hunter, cargobob</label><br />
<label><input type="checkbox" name="t512" />512 military: hydra,rustler</label><br />
</p>
<p><input type="submit" /></p>
</form>
</body>
</html>
