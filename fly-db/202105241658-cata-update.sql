UPDATE apt SET x=-33853.6250,y=-967.0604 WHERE i=9;

UPDATE msp SET x=-33789.7773,y=-1468.7184,z=8.4110 WHERE a=9 AND name='cargo A';
UPDATE msp SET x=-33570.8477,y=-1548.2699,z=8.4110 WHERE a=9 AND name='cargo B';
UPDATE msp SET x=-34089.4219,y=-1349.5988,z=8.4110 WHERE a=9 AND name='gate 1';
UPDATE msp SET x=-34037.6094,y=-1370.2334,z=8.4110 WHERE a=9 AND name='gate 2';
UPDATE msp SET x=-33964.4414,y=-1397.2913,z=8.4110 WHERE a=9 AND name='gate 3';
UPDATE msp SET x=-33913.5977,y=-1413.4023,z=8.4110 WHERE a=9 AND name='gate 4';
UPDATE msp SET x=-34162.1094,y=-1370.3408,z=8.4110 WHERE a=9 AND name='gate P1';
UPDATE msp SET x=-34201.2695,y=-1356.4204,z=8.4110 WHERE a=9 AND name='gate P2';
UPDATE msp SET x=-34240.4844,y=-1342.5933,z=8.4110 WHERE a=9 AND name='gate P3';
UPDATE msp SET x=-34178.9883,y=-1417.7224,z=10.8216 WHERE a=9 AND name='H1';
UPDATE msp SET x=-34218.3516,y=-1403.2502,z=10.8216 WHERE a=9 AND name='cargo H';

INSERT INTO msp(a,x,y,z,t,name) VALUES
	(9,-34257.6016,-1388.9994,10.8216,64,'H2');

UPDATE svp SET x=-34428.5898,y=-1202.8051,z=8.3909 WHERE id=10;
UPDATE svp SET x=-34449.5898,y=-1261.7131,z=8.4179 WHERE id=11;

-- helipad, this was deleted but never written down in these files?
DELETE FROM rnw WHERE a=9 AND i=1;

-- new 3rd helipad
INSERT INTO rnw (a,i,s,h,x,y,z,w,n,type,surf) VALUES
(9,1,'H',0,-34257.6016,-1388.9994,10.8216,25,0,2,3);

-- move helipads
UPDATE rnw SET x=-34178.9883,y=-1417.7224 WHERE a=9 AND i=2;
UPDATE rnw SET x=-34218.3516,y=-1403.2502 WHERE a=9 AND i=3;

-- move runway
UPDATE rnw SET x=-33517.2539,y=-1351.5496 WHERE a=9 AND i=0 AND h=290;
UPDATE rnw SET x=-34350.5273,y=-1048.5193 WHERE a=9 AND i=0 AND h=110;
