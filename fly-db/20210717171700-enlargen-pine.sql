-- remove cargo mission point (but first assign all usages to the other mission point)
-- tomsp
UPDATE flg JOIN msp ON msp.i=flg.tmsp JOIN apt ON flg.tapt=apt.i SET flg.tmsp=(SELECT msp.i FROM msp JOIN apt ON apt.i=msp.a WHERE msp.name='gate 1' AND apt.c='PINE') WHERE apt.c='PINE' AND msp.name='cargo';
-- frommsp
UPDATE flg JOIN msp ON msp.i=flg.fmsp JOIN apt ON flg.fapt=apt.i SET flg.fmsp=(SELECT msp.i FROM msp JOIN apt ON apt.i=msp.a WHERE msp.name='gate 1' AND apt.c='PINE') WHERE apt.c='PINE' AND msp.name='cargo';
-- remove it
DELETE msp FROM msp JOIN apt ON msp.a=apt.i WHERE apt.c='PINE' AND msp.name='cargo';

-- add cargo mission type to the other mission point
UPDATE msp JOIN apt ON msp.a=apt.i SET t=0x1B WHERE apt.c='PINE' AND msp.name='gate 1';
-- update PINE beacon location
UPDATE apt SET x=-2674.9,y=-2339.2,z=28.5 WHERE c='PINE';
-- update SVP location
UPDATE svp JOIN apt ON svp.apt=apt.i SET svp.x=-2654.9937,svp.y=-2307.8584,svp.z=13.3046 WHERE apt.c='PINE';
-- update gate 1 location
UPDATE msp JOIN apt ON msp.a=apt.i SET msp.x=-2648.2097,msp.y=-2321.5491,msp.z=13.4546 WHERE apt.c='PINE' AND msp.name='gate 1';
-- update helipad location
UPDATE msp JOIN apt ON msp.a=apt.i SET msp.x=-2601.9822,msp.y=-2299.6726,msp.z=15.1684 WHERE apt.c='PINE' AND msp.name='helipad';
-- update pine14 runway position
UPDATE rnw JOIN apt ON rnw.a=apt.i SET rnw.x=-2764.6,rnw.y=-2153.7,rnw.z=13.68 WHERE apt.c='PINE' AND rnw.h=135;
