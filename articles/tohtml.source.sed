# some unmaintainable markup to html sed script, adapted from github.com/yugecin/php

1 {
	x
	s/^.*$/empty/
	x
}

:begin

/\\$/ {
	N
	s/\\\n//
	b begin
}

/^$/ {
	x
	s/^!//
	x
	d
}

/^<pre>$/ {
	N
	s/\n//
	:pre
	:concatpre
	/\\$/ {
		N
		s/\\\n//
		b concatpre
	}
	n
	/^<\/pre>$/ !{
		b pre
	}
	n
	b begin
}

s/^\s\+//
s/\s\+$//

s/\\}/~ESCAPEDENDTAG~/g

:moretags

s_{@clear}_<div class="clear"></div>_

/^[^}]*{@/ {

	s/{@/~FIRSTTAG~/
	s/{@/~NEXTTAGS~/g
	s/~FIRSTTAG~/{@/

	#SIMPLETAG h1
	#SIMPLETAG h2
	#SIMPLETAG h3
	#SIMPLETAG h4
	#SIMPLETAG h5
	#SIMPLETAG h6
	#SIMPLETAG ul
	#SIMPLETAG li
	#SIMPLETAG code
	#SIMPLETAG blockquote
	#SIMPLETAG sup
	#SIMPLETAG sub

	/{@b / {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/strong>/
		x
		s/.*\n\(.*\){@b \(.*\)/\1<strong>\2/
		b nexttag
	}

	/{@u / {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/span>/
		x
		s/.*\n\(.*\){@u \(.*\)/\1<span class="u">\2/
		b nexttag
	}

	/{@em / {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/em>/
		x
		s/.*\n\(.*\){@em \(.*\)/\1<em>\2/
		b nexttag
	}

	/{@small\( \|$\)/ {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/span>/
		x
		s/.*\n\(.*\){@small \?\(.*\)/\1<span class="small">\2/
		b nexttag
	}

	/{@s\( \|$\)/ {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/span>/
		x
		s/.*\n\(.*\){@s \?\(.*\)/\1<span class="s">\2/
		b nexttag
	}

	/{@caption / {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/p>/
		x
		s/.*\n\(.*\){@caption \(.*\)/\1<p class="capt">\2/
		b nexttag
	}

	/{@infoboxright/ {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/div>/
		x
		s/.*\n\(.*\){@infoboxright\(.*\)/\1<div class="infobox" style="float:right;">\2/
		b nexttag
	}

	s_{@aimg=\([^;]*\);\([^;]*\);\([^;]*\);\(.*\)}_{@img=<?=$STATICPATH?>/articles/\1;\2;\3;\4}_
	s_{@simg=\([^;]*\);\([^;]*\);\([^;]*\);\(.*\)}_{@img=<?=$STATICPATH?>/\1;\2;\3;\4}_
	/{@img=[^;]*;[^;]*;[^;]*;.*}/ {
		s_{@img=\([^;]*\);\([^;]*\);\([^;]*\);\(.*\)}_<p class="img"><img src="\1" alt="\2" title="\3"/><br/>\4</p>_
		b nexttag
	}

	s_{@aimg=\([^;]*\);\([^;]*\);\([^;]*\)}_{@img=<?=$STATICPATH?>/articles/\1;\2;\3}_
	s_{@simg=\([^;]*\);\([^;]*\);\([^;]*\)}_{@img=<?=$STATICPATH?>/\1;\2;\3}_
	/{@img=[^;]*;[^;]*;.*}/ {
		s_{@img=\([^;]*\);\([^;]*\);\(.*\)}_<p class="img"><img src="\1" alt="\2" title="\3"/></p>_
		b nexttag
	}

	/{@ia=[^ ]*\( \|$\)/ {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/a>/
		x
		s_.*\n\(.*\){@ia=\([^ ]*\) \?\(.*\)_\1<a href="<?=$BASEPATH?>/\2">\3_
		b nexttag
	}

	/{@nfa=[^ ]*\( \|$\)/ {
		H
		g
		s_\(.*\)\n.*$_\1\n</a><img src="<?=$STATICPATH?>/moin-www.png" alt="globe icon" title="external link"/>_
		x
		s/.*\n\(.*\){@nfa=\([^ ]*\) \?\(.*\)/\1<a href="\2" rel="nofollow">\3/
		b nexttag
	}

	/{@a=[^ ]*\( \|$\)/ {
		H
		g
		s_\(.*\)\n.*$_\1\n</a><img src="<?=$STATICPATH?>/moin-www.png" alt="globe icon" title="external link"/>_
		x
		s/.*\n\(.*\){@a=\([^ ]*\) \?\(.*\)/\1<a href="\2">\3/
		b nexttag
	}

	/{@#=[^ ]*\( \|$\)/ {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/a>/
		x
		s_.*\n\(.*\){@#=\([^ ]*\) \?\(.*\)_\1<a href="#\2">\3_
		b nexttag
	}

	/{@article=[^ ]*\( \|$\)/ {
		H
		g
		s/\(.*\)\n.*$/\1\n<\/a>/
		x
		s_.*\n\(.*\){@article=\([^ ]*\) \?\(.*\)_\1<a href="article.php?title=\2">\3_
		b nexttag
	}

	/{@hr}/ {
		s_{@hr}_<hr/>_
		b nexttag
	}

	a<h1>unknown tag</h1>
	q

	:nexttag
	s/~NEXTTAGS~/{@/g
	b moretags
}

/}/ {
	x
	/empty$/ {
		x
		a<h1>tag stack is empty!</h1>
		q
	}
	x
	s/}/~FIRSTEND~/
	s/}/~NEXTENDS~/g
	s/~FIRSTEND~/}/
	H
	s/^\([^}]*\)}.*$/\1/
	G
	s/\n[@!]\?empty.*\n\([^\n]\+\)\n.*}\(.*\)$/\1\2/
	x
	s/\n[^\n]*\n[^\n]*$//
	x
	s/~NEXTENDS~/}/g
	b moretags
}

s/~ESCAPEDENDTAG~/}/g

# add <p> tags
# opening tag should be added on lines that either don't start
# with a '<' or start with tags that are allowed inside para's
# (such as '<a>' or '<span>')

# if this condition doesn't match, hold space gets a ! in the
# beginning. this ! means a paragraph can't be inserted.
# the ! is removed again when an empty line is found (this
# happens in the top section of the script where empty lines
# are ditched)

# if the condition does match, hold space gets a @ in the front
# and the opening <p> tag is inserted (unless hold space already
# had a @ symbol)

# adding the closing tag is done by checking if the next line
# is empty and if hold space has a @ symbol, meaning one is
# currently opened and should be closed. if that's the case,
# append a </p> and remove the @ from hold space

# check if <p> can be prepended
/^\(<a\|<span\|<strong\|[^<]\)/ {
	x
	/^!/ {
		x
		n
		b begin
	}
	/^@/ {
		x
		b checkend
	}
	s/^/@/
	x
	s/^/<p>/
	b checkend
}

# cannot be prepended so mark state as such
# (only if not already prepended)
# this will be reset when an empty line is found (see top)
x
/^@/ !{
	/^!/ !{
		s/^/!/
	}
	x
	n
	b begin
}
x

:checkend
# check if </p> should be appended
x
/^@/ {
	# peek next line to see if <p> needs to be closed
	x
	$ {
		# eof, yes (last non-empty line is eof apparently)
		s_$_</p>_
		q
	}
	N
	/\n$/ !{
		# no, print previous pattern space and
		# continue with next
		s/^/~BEGIN~/
		H
		s/^~BEGIN~\(.*\)\n.*$/\1/p
		g
		s/~BEGIN~.*$//
		x
		s/^.*\n//
		b begin
	}

	# yes
	s_\n_</p>_
	x
	s/^@//
}
x
