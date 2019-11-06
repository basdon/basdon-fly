
.PHONY: .templates .aerodromcharts .apfactsheets .articles all

all: .templates .aerodromecharts .apfactsheets .articles

.templates:
	cd templates && make

.aerodromecharts:
	rm static/gen/aerodrome_*
	cd cli && php genaerodromechart.php

.apfactsheets:
	rm articles/apfactsheets/*
	cd cli && php genapfactsheet.php

.articles:
	rm articles/gen/*
	cd articles && make

