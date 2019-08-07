<?php

function spate_default_generate($name)
{
	file_put_contents("../gen/{$name}.php", spate_generate('../templates/', "{$name}.tpl"));
}

/**
 * @param $template_dir directory with trailing slash
 * @param $template name with extension
 * @return parsed template as string
 */
function spate_generate($template_dir, $template)
{
	static $parsed_cache = [];
	static $depth = 0;

	if (++$depth > 128) {
		$result = "recursion depth exceeded";
		goto _return;
	}

	if (isset($parsed_cache[$template])) {
		return $parsed_cache[$template];
	}

	$in = @file_get_contents($template_dir . $template);
	if ($in === false) {
		die("failed reading $template");
	}
	$inlen = strlen($in);

	$m = strlen($in);
	$i = -1;
	$result = '';
	$j = -1;

	$dostring = false;
	$strchar = '';
	$doescape = false;
	$inpre = false;
	$indent = '';
	$preindent = '';
	$doindent = true;
	$dopre = false;

	while (++$i < $m) {
		$wasescape = $doescape;
		$doescape = false;
		$wasindent = $doindent;
		$doindent = false;

		$c = $in[$i];

		switch ($c) {
		case ' ':
		case "\t":
			if (!$wasindent) break;
			$doindent = true;
			if ($dopre && substr($indent, 0, strlen($preindent)) === $preindent) {
				break;
			}
			$indent .= $c;
			goto next;
		case "\r":
			$doindent = true;
			$indent = '';
			if ($dopre) break;
			goto next;
		case "\n":
			$doindent = true;
			$indent = '';
			if ($dopre) break;
			goto next;
		case '\\':
			if ($wasescape) {
				$wasescape = false;
				break;
			}
			$doescape = true;
			goto next;
		case '{':
			if ($wasescape) break;
			$c = $in[++$i];
			$suffix = '){?>';
			if ($c == '$') {
				$result .= '<?=htmlentities($';
				$j += 17;
				$suffix = ',ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE)~>';
				goto directive_parse_conditionbody__start;
			}
			if ($c != '@') {
				$c = '{';
				--$i;
				break;
			}
			$directive = '';
			do {
				$directive .= $c;
				$c = $in[++$i];
			} while ($c != ' ' && $c != '}');
			switch ($directive) {
			case "@pre":
				$dopre = true;
				$preindent = $indent;
				goto next;
			case "@endpre":
				$dopre = false;
				goto next;
			case "@eval":
				$result .= '<?php ';
				$j += 6;
				$suffix = ';?>';
				goto directive_parse_conditionbody__start;
			case "@<?php":
				$result .= '<?php ';
				$j += 6;
				goto next;
			case "@?>":
				$result .= '?>';
				$j += 2;
				goto next;
			case "@unsafe":
				$result .= '<?=';
				$j += 3;
				$suffix = '~>';
				goto directive_parse_conditionbody__start;
			case "@input":
				if ($c == '}') die('empty @input');
				$args = '';
				$c = '';
				do {
					$args .= $c;
					$c = $in[++$i];
				} while ($c != '}');
				$result .= $input = makeinput($args);
				$j += strlen($input);
				goto next;
			case "@urlencode":
				$result .= '<?=urlencode(';
				$j += 13;
				$suffix = ')~>';
				goto directive_parse_conditionbody__start;
			case "@foreach":
				$result .= '<?php foreach(';
				$j += 14;
				goto directive_parse_conditionbody__start;
			case "@else":
				$result .= '<?php }else{ ?>';
				$j += 15;
				goto next;
			case "@elseif":
				$result .= '<?php }else if(';
				$j += 15;
				goto directive_parse_conditionbody__start;
			case "@if":
				$result .= '<?php if(';
				$j += 9;
				goto directive_parse_conditionbody__start;
			case "@try":
				$result .= '<?php try{ ?>';
				$j += 13;
				goto next;
			case "@catch":
				$result .= '<?php }catch(';
				$j += 13;
				goto directive_parse_conditionbody__start;
			case "@endtry":
			case "@endforeach":
			case "@endif":
				$result .= '<?php } ?>';
				$j += 10;
				goto next;
			case "@render":
				$name = '';
render_parse_name:
				$c = $in[++$i];
				if ($c == '}') {
					$r = spate_generate($template_dir, $name);
					$result .= $r;
					$j += strlen($r);
					goto next;
				}
				$name .= $c;
				if ($i >= $inlen - 1) {
					$result .= '<strong>unexpected EOF</strong>';
					goto _return;
				}
				goto render_parse_name;
			default:
				$result[++$j] = '?';
				++$j;
				goto next;
			}
directive_parse_conditionbody__start:
			$quote = '';
			$_esc = false;
directive_parse_conditionbody:
			$c = $in[++$i];
			$__esc = false;
			if (!$_esc && $c == '\\') {
				$__esc = !$_esc;
			} else if (!$_esc && $quote == $c) {
				$quote = '';
			} else if (!$_esc && $quote == '' && ($c == "'" || $c == '"')) {
				$quote = $c;
			} else if ($quote == '' && $c == '}') {
				$result .= $suffix;
				$j += strlen($suffix);
				goto next;
			}
			$_esc = $__esc;
			$result[++$j] = $c;
			if ($i >= $inlen - 1) {
				$result .= '<strong>unexpected EOF</strong>';
				goto _return;
			}
			goto directive_parse_conditionbody;
		}

		if ($wasescape) $result[++$j] = '\\';
		$result[++$j] = $c;
next:

	}

_return:
	$depth--;
	if ($depth == 0) {
		$result = str_replace('?><?php', '', $result);
		$result = str_replace('~>', '?>', $result);
	}
	return /*$parsed_cache[$template] = */$result;
}

/**
 * @param $args arguments of @input
 * @return an input tag
 */
function makeinput($args)
{
	$args = explode(' ', $args);
	$type = $args[0];
	$name = $args[1];
	if ($type == 'submit') {
		$name = '_form';
	}
	$result = '<input type="' . $type . '" name="' . $name . '"';
	if ($type == 'checkbox') {
		$defaultchecked = count($args) > 2 && $args[2] == 'checked';
		$result .= '<?php form_checkbox(\''.$name.'\','.($defaultchecked?1:0).') ~>';
	} else if ($type == 'text') {
		$result .= '<?php form_input(\''.$name.'\') ~>';
	} else if ($type == 'submit') {
		$result .= ' value="' . $args[1] . '"';
	}
	return $result . '/>';
}
