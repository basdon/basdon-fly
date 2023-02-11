<?php

function sanitize_params(&$pars, &$array_destination)
{
	foreach ($pars as $k => $v) {
		if (is_array($v) && !is_null($array_destination)) {
			$array = $pars[$k];
			$dummy = null;
			sanitize_params($array, $dummy);
			$array_destination[$k] = $array;
		}
		if (!is_int($v) && !is_string($v) && !is_float($v)) {
			unset($pars[$k]);
		}
	}
}
$_GET_ARRAY = [];
$_POST_ARRAY = [];
sanitize_params($_GET, $_GET_ARRAY);
sanitize_params($_POST, $_POST_ARRAY);
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
	return $s. number_format($amount, 0, '', ',') . '</span>';
}
