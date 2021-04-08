@ECHO OFF
PAWNCC.EXE -(- -;- -p -d0 -O1 -v2 %* -obasdon.amx basdon.pwn
IF EXIST basdon.xml DEL basdon.xml
