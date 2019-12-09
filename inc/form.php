<?php

function form_checkbox($name, $defaultchecked)
{
	if (isset($_POST['_form'], $_POST[$name]) ||
		($defaultchecked && !isset($_POST['_form'])))
	{
		echo ' checked="checked"';
	}
}

function form_input($name)
{
	if (isset($_POST['_form'], $_POST[$name])) {
		echo ' value="' . htmlentities($_POST[$name], ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE) . '"';
	}
}

function form_value($name)
{
	if (isset($_POST['_form'], $_POST[$name])) {
		echo htmlentities($_POST[$name], ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE);
	}
}

function form_combo($name, $options)
{
	foreach ($options as $k => $v) {
		echo '<option value="'.$k.'"';
		if (isset($_POST['_form'], $_POST[$name]) && $_POST[$name] == $k) {
			echo ' selected="selected"';
		}
		echo '>'.htmlentities($v, ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE).'</option>';
	}
}
