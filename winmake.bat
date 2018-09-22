@ECHO OFF

IF [%~1] EQU [clean] (
	IF EXIST out RD out /S /Q
	MKDIR out
)

SET "PLUGINTYPE=dll"
SET "GCC=S:\broftware\perl32portable\c\bin\gcc.exe"

"K:\cygwin64\bin\make.exe" build

