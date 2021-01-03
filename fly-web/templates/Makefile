SOURCES_FULL := $(wildcard _*.tpl)
OBJECTS_FULL := $(patsubst %.tpl, ../gen/%.php, $(SOURCES_FULL))
SOURCES_PART := $(wildcard ^*.tpl)
OBJECTS_PART := $(patsubst %.tpl, ../gen/%.php, $(SOURCES_PART))

all: $(OBJECTS_FULL) $(OBJECTS_PART)

../gen/%.php: %.tpl
	php make.php $<
