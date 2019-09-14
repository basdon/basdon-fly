
.PHONY: .templates .aerodromcharts .articles all

all: .templates .aerodromecharts .articles

.templates:
	cd templates && make

.aerodromecharts:
	cd cli && php genaerodromechart.php

.articles:
	cd articles && make

