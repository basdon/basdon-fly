<?php

function header_value($value)
{
	return explode("\r", explode("\n", $value)[0])[0];
}
