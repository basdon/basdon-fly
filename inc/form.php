<?php

$form_defaults = [];

function form_checkbox($name, $defaultchecked)
{
	$val = form_prefilledvalue($name);
	if (($val != null && $val) || ($defaultchecked && !isset($_POST['_form']))) {
		echo ' checked="checked"';
	}
}

function form_input($name)
{
	if (($val = form_prefilledvalue($name))) {
		echo ' value="' . htmlentities($val, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE) . '"';
	}
}

function form_value($name)
{
	if (($val = form_prefilledvalue($name))) {
		echo htmlentities($val, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE);
	}
}

function form_combo($name, $options)
{
	foreach ($options as $k => $v) {
		echo '<option value="'.$k.'"';
		if (($val = form_prefilledvalue($name)) && $val == $k) {
			echo ' selected="selected"';
		}
		echo '>'.htmlentities($v, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE).'</option>';
	}
}

function form_prefilledvalue($name)
{
	global $form_defaults;
	if (isset($_POST['_form'], $_POST[$name])) {
		return $_POST[$name];
	}
	if (isset($form_defaults[$name])) {
		return $form_defaults[$name];
	}
	return null;
}
