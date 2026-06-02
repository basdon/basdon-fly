SHELL=bash

# Delete the default suffixes, to get rid of a whole bunch of implicit rules.
# (This doesn't get rid of all of them, but is all we can do from within a Makefile)
# (Running with -r or --no-builtin-rules would disable all)
.SUFFIXES:

# I'd rather not have it print the executed commands but opt for echo'ing a short message inside targets.
# The alternative to using .SILENT is to prefix commands with an @ symbol.
.SILENT:

.PHONY: all
all: so test

###

out/_preprocess: _preprocess.c
	mkdir -p out
	gcc -Wall -x c -std=c89 _preprocess.c -o out/_preprocess
	chmod +x out/_preprocess
	echo recompiled preprocess

__settings.h: out/_preprocess ../settings.ini
	out/_preprocess
	echo regenerated __settings.h

__sampasm.h: out/_preprocess samp.asm
	out/_preprocess
	echo regenerated __sampasm.h

out/samp.o: samp.asm
	nasm -f elf32 samp.asm -o out/samp.o
	echo recompiled samp.asm

# EXTRAFLAGS is intended for optional extra flags passed while invoking make from commandline
ifdef EXTRAFLAGS
$(info using extraflags: $(EXTRAFLAGS))
endif

ifdef BASDONDEV
binaryname = basdonfly-dev
cflags = -DDEV $(EXTRAFLAGS)
else
binaryname = basdonfly-prod
cflags = -O2 $(EXTRAFLAGS)
endif

ofile = out/$(binaryname).o
sofile = out/$(binaryname).so

# This could not be phony, if we'd want to, but that requires listing (and constantly updating)
# all their dependencies, which is work I'd rather not do. Nearly all make invocations will be
# because a source file is updated anyways, so don't bother.
.PHONY: $(ofile)
$(ofile): __settings.h __sampasm.h
	gcc -ggdb -m32 -Wall -Wno-strict-aliasing -fno-pic -c -x c -std=c89 $(cflags) basdonfly.c -o $(ofile)
	echo "compile ok ($(ofile))"

$(sofile): $(ofile) out/samp.o
	gcc -ggdb -m32 -Wall -shared -lm -Xlinker --no-undefined out/samp.o out/$(binaryname).o -o $(sofile)
	echo "link ok ($(sofile))"

out/test_cmd_utils: test/testapi.c test/test_cmd_utils.c cmd_utils.c game_sa_data.c playerdata.h a_samp.h
	gcc -Wall -x c -std=c89 test/test_cmd_utils.c -o out/test_cmd_utils
	chmod +x out/test_cmd_utils
	echo recompiled out/test_cmd_utils

out/test_splitclientmessage: test/testapi.c test/test_splitclientmessage.c util_splitclientmessage.c
	gcc -Wall -x c -std=c89 test/test_splitclientmessage.c -o out/test_splitclientmessage
	chmod +x out/test_splitclientmessage
	echo recompiled out/test_splitclientmessage

.PHONY: test
test: out/test_splitclientmessage out/test_cmd_utils
	out/test_splitclientmessage
	out/test_cmd_utils
	echo test ok

.PHONY: so
so: out/$(binaryname).so
