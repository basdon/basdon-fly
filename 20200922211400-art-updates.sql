INSERT INTO artcat(id,parent,name,color) VALUES (8,NULL,'News','ffd2ad');
INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='News'), 'Update_200922', 'Sep 22 - Mission map, panel');
