-- update pine14 runway position
UPDATE rnw JOIN apt ON rnw.a=apt.i SET rnw.x=-2791.0417,rnw.y=-2126.3020 WHERE apt.c='PINE' AND rnw.h=135;
