# _*.tpl are full pages, ^*.tpl are partial pages
# Other tpl files are always included in the full/partial pages,
# so they do not need to be parsed on their own.
#
TEMPLATE_SOURCES_FULL := $(wildcard templates/_*.tpl)
TEMPLATE_OBJECTS_FULL := $(patsubst templates/%.tpl, gen/%.php, $(TEMPLATE_SOURCES_FULL))
TEMPLATE_SOURCES_PART := $(wildcard templates/^*.tpl)
TEMPLATE_OBJECTS_PART := $(patsubst templates/%.tpl, gen/%.php, $(TEMPLATE_SOURCES_PART))

ARTICLE_SOURCES := $(wildcard articles/*.txt)
ARTICLE_OBJECTS := $(patsubst articles/%.txt, gen/article_%.html, $(ARTICLE_SOURCES))

.PHONY: templates aerodromcharts apfactsheets apvehicelist articles ensureplayergraph islandmaps flightmapjs clean cleantemplates cleanarticles
default: templates aerodromecharts apfactsheets apvehiclelist articles ensureplayergraph islandmaps flightmapjs

cleantemplates:
	rm -f gen/_*.php
	rm -f gen/^*.php
gen/%.php: templates/%.tpl cleantemplates
	cd cli && php parsetemplate.php $<
templates: $(TEMPLATE_OBJECTS_FULL) $(TEMPLATE_OBJECTS_PART)
	@# Regenerating templates resets their permissions,
	@# which means the reparsing them in dev (with $__REPARSE__ in conf.php) will fail.
	@# The SGID bit should be set on the gen folder,
	@# then this will add write permissions for the httpd user again.
	[ -z "$(BASDONDEV)" ] || chmod g+w gen/*

cleanarticles:
	rm -f gen/article_*
	rm -f gen/articlemap.php
	rm -f gen/beaconlist.html
gen/tohtml.sed: articles/tohtml.source.sed articles/tohtml.gen.sed
	sed -f articles/tohtml.gen.sed < articles/tohtml.source.sed > gen/tohtml.sed
gen/article_%.html: articles/%.txt cleanarticles gen/tohtml.sed
	sed -f gen/tohtml.sed < $< > $@
articles: $(ARTICLE_OBJECTS)
	cd cli && php genarticlemap.php
	cd cli && php genbeaconlist.php
	cd cli && php ensureairportcodearticlelinking.php

aerodromecharts:
	rm -f gen/aerodrome*
	rm -f static/gen/aerodrome*
	cd cli && php genaerodromechart.php

apfactsheets:
	rm -f gen/apfactsheet*
	cd cli && php genapfactsheet.php

apvehiclelist:
	rm -f gen/apvehiclelist*
	cd cli && php genapvehiclelist.php

islandmaps:
	rm -f gen/islandmap*
	rm -f static/gen/islandmap*
	cd cli && php genislandmaps.php

flightmapjs: islandmaps
	rm -f static/gen/fm_complete.js
	cd cli && php genflightmapjscomplete.php

ensureplayergraph:
	@# The player graph should only created by the cron job,
	@# but it can be deleted with a 'clean'.
	@# Generate one here if it doesn't exist.
	[ -f "gen/playergraph.png" ] || (cd cli && php genplayergraph.php)

clean:
	rm -f gen/*
	rm -f static/gen/*