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
