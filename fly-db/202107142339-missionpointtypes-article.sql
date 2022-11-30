INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Missions'),'Mission_point_types','Mission point types');
