basdon.net aviation server gamemode plugin.

The codebase is a slight mess because things change (code style, methods, ..),
but who wants to spend time on refactoring right? (well, me,
but refactoring 24/7 without actually having something seems useless)

The fly-gm gamemode script is still used as glue,
but this is meant be removed in the future.

Building requires Linux with nasm, gcc
Building a dev build: . build -DDEV
Building a prod build: . build
Output in out/
