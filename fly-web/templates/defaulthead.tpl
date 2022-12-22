<link rel="stylesheet" href="/s/basdon.css?v13" type="text/css" title="basdon default theme" />
<link rel="icon" type="image/png" sizes="16x16 32x32 48x48" href="/s/favicon.ico?4"/>
<link rel="icon" type="image/png" sizes="96x96" href="/s/favicon-96x96.png">
{@foreach $META_TAGS as $prop => $content}
	<meta property="{@unsafe $prop}" content="{@unsafe $content}" />
{@endforeach}
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<meta charset="utf-8" />
