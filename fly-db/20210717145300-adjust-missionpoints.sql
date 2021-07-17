-- Since one missionpoint can now be for different classes, and there's a table to see what airport has what types,
-- this should normalize/fix/add missing ones that should be available.

-- CALI's single missionpoint didn't have cargo (and possibly also no passenger helicopter) (and was named 'gate 2'?)
UPDATE msp JOIN apt ON apt.i=msp.a SET msp.name='gate 1',msp.t=0xC9 WHERE apt.c='CALI';

-- LSIF's single missionpoint didn't have cargo nor heli
UPDATE msp JOIN apt ON apt.i=msp.a SET msp.t=0xC9 WHERE apt.c='LSIF';

-- IGZU's single heli missionpoint was passenger only
UPDATE msp JOIN apt ON apt.i=msp.a SET msp.t=0xC0 WHERE apt.c='IGZU' AND msp.name='H1';

-- PINE's single heli missionpoint was passenger only
UPDATE msp JOIN apt ON apt.i=msp.a SET msp.t=0xC0 WHERE apt.c='PINE' AND msp.name='helipad';

-- SALS's single heli missionpoint was passenger only
UPDATE msp JOIN apt ON apt.i=msp.a SET msp.t=0xC0 WHERE apt.c='SALS' AND msp.name='H1';
