INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Basics'), 'HUD', 'HUD');
INSERT INTO artcat(id,parent,name,color) VALUES (6,NULL,'Missions','ccffcc'), (7,NULL,'Navigation','ccffff');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Missions'), 'Passenger_Satisfaction', 'Passenger Satisfaction');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Missions'), 'Missions', 'Missions');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Navigation'), 'List_Of_Beacons', 'List of beacons');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Navigation'), 'ADF', 'ADF (Automatic Direction Finder)');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Navigation'), 'VOR', 'VOR (VHF Omnidirectional Range)');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Navigation'), 'ILS', 'ILS (Instrument Landing System)');
