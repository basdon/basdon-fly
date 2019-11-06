
{@rem expect array $articles @}

{@eval $bull = ''}
{@foreach $articles as $a}
	{@unsafe $bull}<a href="article.php?title={@unsafe $a->name}">{$a->title}</a>
	{@eval $bull = ' &bull; '}
{@endforeach}
