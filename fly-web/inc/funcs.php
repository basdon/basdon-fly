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
