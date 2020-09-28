INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='News'), 'Update_200928', 'Sep 28 - Website flight map');
