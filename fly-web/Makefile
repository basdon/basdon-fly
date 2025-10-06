# _*.tpl are full pages, ^*.tpl are partial pages
# Other tpl files are always included in the full/partial pages,
# so they do not need to be parsed on their own.
#
TEMPLATE_SOURCES_FULL := $(wildcard templates/_*.tpl)
TEMPLATE_OBJECTS_FULL := $(patsubst templates/%.tpl, gen/%.php, $(TEMPLATE_SOURCES_FULL))
TEMPLATE_SOURCES_PART := $(wildcard templates/^*.tpl)
TEMPLATE_OBJECTS_PART := $(patsubst templates/%.tpl, gen/%.php, $(TEMPLATE_SOURCES_PART))

.PHONY: templates aerodromcharts all_airport_codes apfactsheets apvehicelist \
	articles ensureplayergraph islandmapdata islandmaps flightmapjs \
	clean cleantemplates cleanarticles \
	airportlist navinfotable mission-point-types-tables
default: templates aerodromecharts all_airport_codes apfactsheets apvehiclelist \
	articles ensureplayergraph islandmapdata islandmaps flightmapjs \
	airportlist navinfotable mission-point-types-tables

inc/__settings.php: ../settings.ini cli/gensettings.php
	cd cli && php gensettings.php
	@# Regenerating the file resets the file's permissions,
	@# which means the reparsing them in dev (with $__REPARSE_TEMPLATES_EVERY_PAGE_LOAD__ in conf.php) will fail.
	@# The SGID bit should be set on the inc folder,
	@# then this will add write permissions for the httpd user again.
	[ -z "$(BASDONDEV)" ] || chmod g+w inc/__settings.php

cleantemplates:
	rm -f templates/gen/*.php
gen/%.php: templates/%.tpl cleantemplates
	cd cli && php parsetemplate.php $<
templates: $(TEMPLATE_OBJECTS_FULL) $(TEMPLATE_OBJECTS_PART)
	@# Regenerating templates resets their permissions,
	@# which means the reparsing them in dev (with $__REPARSE_TEMPLATES_EVERY_PAGE_LOAD__ in conf.php) will fail.
	@# The SGID bit should be set on the templates/gen folder,
	@# then this will add write permissions for the httpd user again.
	[ -z "$(BASDONDEV)" ] || chmod g+w templates/gen/*

cleanarticles:
	rm -f gen/articlemap.php
	rm -f gen/beaconlist.html
articles: inc/__settings.php
	cd cli && php genarticlemap.php
	cd cli && php genbeaconlist.php
	cd cli && php ensureairportcodearticlelinking.php

aerodromecharts: inc/__settings.php
	rm -f gen/aerodrome*
	rm -f www/s/gen/aerodrome*
	cd cli && php genaerodromechart.php

all_airport_codes:
	rm -f inc/gen/all_airport_codes.php
	cd cli && php gen_all_airport_codes.php

apfactsheets: inc/__settings.php
	rm -f gen/apfactsheet*
	cd cli && php genapfactsheet.php

apvehiclelist: inc/__settings.php
	rm -f gen/apvehiclelist*
	cd cli && php genapvehiclelist.php

islandmapdata:
	rm -f gen/islandmapdata.txt
	cd cli && php genislandmapdata.php

islandmaps: inc/__settings.php
	rm -f gen/islandmap*.html
	rm -f www/s/gen/islandmap*
	cd cli && php genislandmaps.php

flightmapjs: islandmapdata inc/__settings.php
	rm -f www/s/gen/fm_complete.js
	cd cli && php genflightmapjscomplete.php

ensureplayergraph: inc/__settings.php
	@# The player graph should only created by the cron job,
	@# but it can be deleted with a 'clean'.
	@# Generate one here if it doesn't exist.
	[ -f "gen/playergraph.png" ] || (cd cli && php genplayergraph.php)

airportlist: inc/__settings.php
	cd cli && php genairportlist.php

navinfotable: inc/__settings.php
	cd cli && php gennavinfotable.php

mission-point-types-tables: inc/__settings.php
	cd cli && php gen-mission-point-types-airports.php
	cd cli && php gen-mission-point-types-classes.php
	cd cli && php gen-mission-point-types-vehicles.php

clean:
	rm -f inc/__settings.php
	rm -f inc/gen/*
	rm -f gen/*
	rm -f templates/gen/*
	rm -f www/s/gen/*
