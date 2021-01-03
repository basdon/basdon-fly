-- LVA
-- cargo B back for planes
UPDATE msp SET t=56,name='cargo B' WHERE i=6;
-- H1 to cargo
UPDATE msp SET t=128,name='cargo H' WHERE i=8;
-- other H names
UPDATE msp SET name='H1' WHERE i=7;
UPDATE msp SET name='H2' WHERE i=9;

-- OCTA
UPDATE msp SET name='cargo A' WHERE i=36;
UPDATE msp SET name='gate 1' WHERE i=37;
UPDATE msp SET name='gate 2' WHERE i=38;
UPDATE msp SET name='cargo H' WHERE i=39;

-- SFA
-- cargo H back to heli
UPDATE msp SET t=64,name='H2' WHERE i=23;
-- new cargo h
INSERT INTO msp(a,x,y,z,t,name) VALUES ((SELECT i FROM apt WHERE c='SASF'),-1356.8170,-427.4277,13.9000,128,'cargo H');

-- LVA
-- new cargo h
INSERT INTO msp(a,x,y,z,t,name) VALUES ((SELECT i FROM apt WHERE c='SALS'),1968.0000,-2270.0000,13.3000,128,'cargo H');
