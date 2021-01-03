INSERT INTO artalt(art,alt) VALUES
((SELECT id FROM art WHERE name='Area_69'),'BASE'),
((SELECT id FROM art WHERE name='Verdant_Meadows'),'SAVM'),
((SELECT id FROM art WHERE name='San_Fierro_Carrier'),'SFCA'),
((SELECT id FROM art WHERE name='Los_Santos_Inlet_Field'),'LSIF'),
((SELECT id FROM art WHERE name='Caligulas_Palace'),'CALI');
