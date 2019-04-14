<?php

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
				$result .= '<?php echo htmlentities($';
				$j += 25;
				$suffix = ',ENT_QUOTES|ENT_HTML5|ENT_SUBSTITUTE);?>';
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
			echo '-';
			echo $indent;
			echo '-';
				$dopre = true;
				$preindent = $indent;
				goto next;
			case "@endpre":
				$dopre = false;
				goto next;
			case "@eval":
				$result .= '<?php ';
				$j += 6;
				$suffix = '?>';
				goto directive_parse_conditionbody__start;
			case "@unsafe":
				$result .= '<?= ';
				$j += 4;
				$suffix = ' ?>';
				goto directive_parse_conditionbody__start;
			case "@foreach":
				$result .= '<?php foreach(';
				$j += 14;
				goto directive_parse_conditionbody__start;
			case "@elseif":
				$result .= '<?php }else if(';
				$j += 15;
				goto directive_parse_conditionbody__start;
			case "@if":
				$result .= '<?php if(';
				$j += 9;
				goto directive_parse_conditionbody__start;
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
	$result = str_replace('?><?php', '', $result);
	return $parsed_cache[$template] = $result;
}
