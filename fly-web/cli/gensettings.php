<?php
// treat all notices/warnings/errors as errors
function exception_error_handler($severity, $message, $file, $line)
{
	throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

$lines = file('../../settings.ini', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
if ($lines === false) {
	exit(1);
}
$content = "<?php\n";
for ($i = 0; $i < count($lines); $i++) {
	$line = $lines[$i];
	if ($line[0] != ';') {
		while ($line[strlen($line) - 1] == '\\') {
			$line = substr($line, 0, -1);
			$i++;
			if ($i < count($lines)) {
				$line .= $lines[$i];
				continue;
			}
			break;
		}
		$line = str_replace('`', '$SETTING__', $line);
		$content .= '$SETTING__' . $line . ";\n";
	}
}
if (file_put_contents('../inc/__settings.php', $content) === false) {
	echo 'failed writing settings file';
	exit(1);
}
