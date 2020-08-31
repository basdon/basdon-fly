#!/usr/bin/env bash
mkdir -p out
rm out/*

echo BASDON_EXTRA_COMPILATION_OPTIONS=$BASDON_EXTRA_COMPILATION_OPTIONS

(nasm -f elf32 samphost.asm -o out/samphost.o &&
gcc -Wall -m32 -std=c89 $BASDON_EXTRA_COMPILATION_OPTIONS -x c -c basdonfly.c -o out/basdonfly.o &&
#ld -m elf_i386 -shared --no-undefined -o out/basdonfly.so out/samphost.o out/basdonfly.o) ||
gcc -Wall -m32 -shared -lm -Xlinker --no-undefined -o out/basdonfly.so out/samphost.o out/basdonfly.o) ||
(echo "" && echo "" && echo "FAILURE")
