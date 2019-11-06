
{@rem -
	artice
	------
	string name
	string title
	int id
	
	category
	---------
	string name
	string color
	article[] articles
	category[] subs

	expect category $cat

	uncategorized articles should be in a category with null name/color
	categories that have subcategories may not have articles
@}

<table class="artcat">
	<thead>
		<tr>
			<th colspan="2" style="background:#{@unsafe $cat->color}">
				{@eval $gt=''}
				Category: <a href="article.php?category={@urlencode $cat->name}">{@unsafe $cat->name}</a>
			</th>
		</tr>
	</thead>
	<tbody>
		{@if count($cat->articles)}
			<tr>
				<td></td>
				<td>{@eval $articles = $cat->articles}{@render article_category_box_articles.tpl}</td>
			</tr>
		{@endif}
		{@foreach $cat->subs as $c}
			<tr>
				<td style="background:#{@unsafe $c->color}">
					<a href="article.php?category={@unsafe $c->name}">{@unsafe $c->name}</a>
				</td>
				<td>{@eval $articles = $c->articles}{@render article_category_box_articles.tpl}</td>
			</tr>
		{@endforeach}
	</tbody>
</table>
