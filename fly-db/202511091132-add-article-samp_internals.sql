INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='Internals'),'Samp_Internals','Samp internals');
