#!/usr/bin/env bash
mkdir -p out
rm out/*


echo opts: $1 $2 $3 $4 $5 $6 $7

(gcc -Wall -std=c89 -x c _preprocess.c -o out/_preprocess &&
chmod +x out/_preprocess && out/_preprocess &&
nasm -f elf32 samp.asm -o out/samp.o &&
gcc -Wall -Wno-strict-aliasing -fno-pic -m32 -std=c89 $1 $2 $3 $4 $5 $6 $7 -x c -c basdonfly.c -o out/basdonfly.o &&
#ld -m elf_i386 -shared --no-undefined -o out/basdonfly.so out/samphost.o out/basdonfly.o) ||
gcc -Wall -m32 -shared -lm -Xlinker --no-undefined -o out/basdonfly.so out/samp.o out/basdonfly.o) ||
(echo "" && echo "" && echo "FAILURE")
