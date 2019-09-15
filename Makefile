
.PHONY: .templates .aerodromcharts .apfactsheets .articles all

all: .templates .aerodromecharts .apfactsheets .articles

.templates:
	cd templates && make

.aerodromecharts:
	cd cli && php genaerodromechart.php

.apfactsheets:
	cd cli && php genapfactsheet.php

.articles:
	cd articles && make

