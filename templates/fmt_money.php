<?php 
function fmt_money($amount)
{
	$s = '<span style="color:#';
	if ($amount < 0) {
		$s .= 'c22">-$';
		$amount = -$amount;
	} else {
		$s .= '2a2">$';
	}
	$amount = "$amount";
	$l = $len = strlen($amount);
	while ($l--) {
		$s .= $amount[$len - $l - 1];
		if ($l && $l % 3 == 0) {
			$s .= ',';
		}
	}
	return $s . '</span>';
}
?>