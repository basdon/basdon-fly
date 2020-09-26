#!/usr/bin/env bash
mkdir -p out
rm out/*

echo opts: $1 $2 $3 $4 $5 $6 $7

(nasm -f elf32 samphost.asm -o out/samphost.o &&
gcc -Wall -Wno-strict-aliasing -m32 -std=c89 $1 $2 $3 $4 $5 $6 $7 -x c -c basdonfly.c -o out/basdonfly.o &&
#ld -m elf_i386 -shared --no-undefined -o out/basdonfly.so out/samphost.o out/basdonfly.o) ||
gcc -Wall -m32 -shared -lm -Xlinker --no-undefined -o out/basdonfly.so out/samphost.o out/basdonfly.o) ||
(echo "" && echo "" && echo "FAILURE")
