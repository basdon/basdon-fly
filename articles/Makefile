SOURCES := $(wildcard *.txt)
OBJECTS := $(patsubst %.txt, gen/%.html, $(SOURCES))

all: $(OBJECTS)

tohtml.sed: tohtml.source.sed tohtml.gen.sed
	sed -f tohtml.gen.sed < tohtml.source.sed > tohtml.sed

gen/%.html: %.txt tohtml.sed
	sed -f tohtml.sed < $< > $@
