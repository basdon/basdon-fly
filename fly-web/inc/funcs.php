<?php

function sanitize_params(&$pars)
{
	foreach ($pars as $k => $v) {
		if (!is_int($v) && !is_string($v) && !is_float($v)) {
			unset($pars[$k]);
		}
	}
}
sanitize_params($_GET);
sanitize_params($_POST);
// $_FILES?

function header_value($value)
{
	return explode("\r", explode("\n", $value)[0])[0];
}

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
