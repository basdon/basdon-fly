Basdon.net aviation server gamemode plugin.

The codebase is a slight mess because things change (code style, methods, ..),
but who wants to spend time on refactoring right? (well, me,
but refactoring 24/7 without actually having something seems useless)

The fly-gm gamemode script is still used as glue,
but this is meant be removed in the future.

Building requires Linux with nasm, gcc
Building a dev build: . devmake.sh
                  or: . nixmake.sh -DDEV
Building a prod build: . nixmake.sh -O3
Output in out/

Database stuff is in fly-db repository.

Place textdraw resources in serverpath scriptfiles/texts
Place map resources in serverpath scriptfiles/maps

Resources are not uploaded (yet?), to be determined.

Used plugins are
  bcrypt-samp v2.2 (github.com/lassir/bcrypt-samp)
  mysql r39.3 (github.com/pBlueG/SA-MP-MySQL)
  simplesocket (github.com/basdon/simplesocket) (to be embedded)

This project is licensed under the GNU AGPLv3 License.
Files in vendor/ do NOT fall under this License.

Files in vendor/SDK/amx are Copyright (c) ITB CompuPhase, 1997-2005
Files in vendor/SDK (excluding the amx/ directory) are Copyright 2004-2009
SA-MP Team
Modifications in these files are marked as such with comments.
