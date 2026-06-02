basdon.net aviation server gamemode plugin.

The codebase is a slight mess because things change (code style, methods, ..),
but who wants to spend time on refactoring right? (well, me,
but refactoring 24/7 without actually having something seems useless)

The fly-gm gamemode script is still used as glue,
but this is meant be removed in the future.

Building requires Linux with nasm, gcc

Building: run 'make'
If a BASDONDEV environment variable is non-empty, a production build will be made
with extra optimization without development code.
Contents of an optional EXTRAFLAGS environment variable is passed to gcc during
compilation phase.
Output will be out/basdonfly-dev.so or out/basdonfly-prod.so

Running 'make' will compile the .so and run tests.
These two actions can also be done on their own:
- 'make so' compiles the .so
- 'make test' compiles (if needed) and runs the test
but it's recommended to just run 'make' when a new .so is required,
because recompiling tests will only happen if they're outdated and
running them takes little time.
