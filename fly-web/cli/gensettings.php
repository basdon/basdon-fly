<?php

$lines = file('../../settings.ini', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
if ($lines === false) {
	exit(1);
}
$content = "<?php\n";
foreach ($lines as $line) {
	if ($line[0] != '#') {
		$idx = strpos($line, ' ');
		if ($idx === false) {
			echo "invalid settings.ini line: $line";
			exit(1);
		}
		$content .= '$SETTING__' . substr($line, 0, $idx) . '=' . substr($line, $idx + 1) . ";\n";
	}
}
if (file_put_contents('../inc/__settings.php', $content) === false) {
	echo 'failed writing settings file';
	exit(1);
}
