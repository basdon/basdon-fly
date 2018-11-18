<?php

/**
 * @param $template_dir directory with trailing slash
 * @param $template name with extension
 * @return parsed template as string
 */
function spate_generate($template_dir, $template)
{
	static $parsed_cache = [];

	if (isset($parsed_cache[$template])) {
		return $parsed_cache[$template];
	}

	$in = @file_get_contents($template_dir . $template);
	if ($in === false) {
		die("failed reading $template");
	}

	$m = strlen($in);
	$i = -1;
	$result = '';
	$j = -1;

	$dostring = false;
	$strchar = '';
	$doescape = false;
	$inpre = false;
	$indent = '';
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
			$indent .= $c;
			goto next;
		case "\r":
			$doindent = true;
			goto next;
		case "\n":
			$doindent = true;
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
			case "@elseif":
				$result .= '<?php }else if(';
				$j += 15;
				goto directive_parse_if__start;
			case "@if":
				$result .= '<?php if(';
				$j += 9;
directive_parse_if__start:
				$quote = '';
directive_parse_if:
				$c = $in[++$i];
				if ($quote == $c) {
					$quote == '';
				} else if ($quote == '' && ($c == "'" || $c == '"')) {
					$quote = $c;
				} else if ($c == '}') {
					$result .= '){?>';
					$j += 4;
					goto next;
				}
				$result[++$j] = $c;
				goto directive_parse_if;
			case "@endif":
				$result .= '<?php } ?>';
				$j += 10;
			default:
				$result[++$j] = '?';
			}
		}

		if ($wasescape) $result[++$j] = '\\';
		$result[++$j] = $c;
next:
	}

	$result = str_replace('?><?php', '', $result);
	return $parsed_cache[$template] = $result;
}
