
.PHONY: .templates .aerodromcharts .apfactsheets .articles all

all: .templates .aerodromecharts .apfactsheets .articles

.templates:
	cd templates && make

.aerodromecharts:
	rm -f static/gen/aerodrome_*
	cd cli && php genaerodromechart.php

.apfactsheets:
	rm -f articles/apfactsheets/*
	cd cli && php genapfactsheet.php

.articles:
	cd cli && php genarticlemapping.php
	rm -f articles/gen/*
	cd articles && make

