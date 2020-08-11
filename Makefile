
.PHONY: .templates .aerodromcharts .apfactsheets .articles .islandmaps all

all: .templates .aerodromecharts .apfactsheets .articles .islandmaps

.templates:
	rm -f gen/*
	cd templates && make

.aerodromecharts:
	rm -f static/gen/aerodrome*
	cd cli && php genaerodromechart.php

.apfactsheets:
	rm -f articles/apfactsheets/*
	cd cli && php genapfactsheet.php
	cd cli && php genapvehiclelist.php

.articles:
	rm -f articles/gen/*
	cd cli && php genarticlemapping.php
	cd cli && php genbeaconlist.php
	cd cli && php ensureairportcodearticlelinking.php
	cd articles && make

.islandmaps:
	rm -f articles/islandmaps/*
	rm -f static/gen/islandmap*
	cd cli && php genislandmaps.php
