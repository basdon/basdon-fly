
{@rem expect array $articles @}

{@eval $bull = ''}
{@foreach $articles as $a}
	{@unsafe $bull}
	{@if isset($article_id) && $a->id == $article_id}
		<strong>{@unsafe $a->title}</strong>
	{@else}
		<a href="article.php?title={@unsafe $a->name}">{@unsafe $a->title}</a>
	{@endif}
	{@eval $bull = ' &bull; '}
{@endforeach}
