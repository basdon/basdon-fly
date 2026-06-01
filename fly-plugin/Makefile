SHELL=bash

# Delete the default suffixes, to get rid of a whole bunch of implicit rules.
# (This doesn't get rid of all of them, but is all we can do from within a Makefile)
# (Running with -r or --no-builtin-rules would disable all)
.SUFFIXES:

# I'd rather not have it print the executed commands but opt for echo'ing a short message inside targets.
# The alternative to using .SILENT is to prefix commands with an @ symbol.
.SILENT:

.PHONY: default build_so_and_test
default: build_so_and_test

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

# This could not be phony, if we'd want to, but that requires listing (and constantly updating)
# all their dependencies, which is work I'd rather not do. Nearly all make invocations will be
# because a source file is updated anyways, so don't bother.
.PHONY: out/$(binaryname).o
out/$(binaryname).o: __settings.h __sampasm.h
	gcc -ggdb -m32 -Wall -Wno-strict-aliasing -fno-pic -c -x c -std=c89 $(cflags) basdonfly.c -o out/$(binaryname).o
	echo "compile ok ($(binaryname))"

out/$(binaryname).so: out/$(binaryname).o out/samp.o
	gcc -ggdb -m32 -Wall -shared -lm -Xlinker --no-undefined out/samp.o out/$(binaryname).o -o out/$(binaryname).so
	echo "link ok ($(binaryname))"

out/testmain: test/testmain.c test/test_splitclientmessage.c util_splitclientmessage.c test/test_cmd_utils.c cmd_utils.c game_sa_data.c playerdata.h a_samp.h
	gcc -Wall -x c -std=c89 test/testmain.c -o out/testmain
	chmod +x out/testmain
	echo recompiled testmain

build_so_and_test: out/$(binaryname).so out/testmain
	out/testmain
	echo test ok
